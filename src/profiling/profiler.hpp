#pragma once

#include <algorithm>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>

#include <cassert>
#include <cstdlib>

#include <json/json.hpp>

#include <threading/threading.hpp>
#include <util.hpp>

namespace nest {
namespace mc {
namespace util {

inline std::string green(std::string s)  { return s; }
inline std::string yellow(std::string s) { return s; }
inline std::string white(std::string s)  { return s; }
inline std::string red(std::string s)    { return s; }
inline std::string cyan(std::string s)   { return s; }

using timer_type = nest::mc::threading::timer;

namespace impl {
    /// simple hashing function for strings
    static inline
    size_t hash(const char* s) {
        size_t h = 5381;
        while (*s) {
            h = ((h << 5) + h) + int(*s);
            ++s;
        }
        return h;
    }

    /// std::string overload for hash
    static inline
    size_t hash(const std::string& s) {
        return hash(s.c_str());
    }
} // namespace impl

/// The tree data structure that is generated by post-processing of
/// a profiler.
struct profiler_node {
    double value;
    std::string name;
    std::vector<profiler_node> children;
    using json = nlohmann::json;

    profiler_node() :
        value(0.), name("")
    {}

    profiler_node(double v, const std::string& n) :
        value(v), name(n)
    {}

    void print(int indent=0);
    void print(std::ostream& stream, double threshold);
    void print_sub(std::ostream& stream, int indent, double threshold, double total);
    void fuse(const profiler_node& other);
    /// return wall time spend in "other" region
    double time_in_other() const;
    /// scale the value in each node by factor
    /// performed to all children recursively
    void scale(double factor);

    json as_json() const;
};

profiler_node operator+ (const profiler_node& lhs, const profiler_node& rhs);
bool operator== (const profiler_node& lhs, const profiler_node& rhs);

// a region in the profiler, has
// - name
// - accumulated timer
// - nested sub-regions
class region_type {
    region_type *parent_ = nullptr;
    std::string name_;
    size_t hash_;
    std::unordered_map<size_t, std::unique_ptr<region_type>> subregions_;
    timer_type::time_point start_time_;
    double total_time_ = 0;

public:

    explicit region_type(std::string n) :
        name_(std::move(n)),
        hash_(impl::hash(n)),
        start_time_(timer_type::tic())
    {}

    explicit region_type(const char* n) :
        region_type(std::string(n))
    {}

    region_type(std::string n, region_type* p) :
        region_type(std::move(n))
    {
        parent_ = p;
    }

    const std::string& name() const { return name_; }
    void name(std::string n) { name_ = std::move(n); }

    region_type* parent() { return parent_; }

    void start_time() { start_time_ = timer_type::tic(); }
    void end_time  () { total_time_ += timer_type::toc(start_time_); }
    double total() const { return total_time_; }

    bool has_subregions() const { return subregions_.size() > 0; }

    void clear() {
        subregions_.clear();
        start_time();
    }

    size_t hash() const { return hash_; }

    region_type* subregion(const char* n);

    double subregion_contributions() const;

    profiler_node populate_performance_tree() const;
};

class profiler {
public:
    profiler(std::string name) :
        root_region_(std::move(name))
    {}

    // the copy constructor doesn't do a "deep copy"
    // it simply creates a new profiler with the same name
    // This is needed for tbb to create a list of thread local profilers
    profiler(const profiler& other) :
        profiler(other.root_region_.name())
    {}

    /// step down into level with name
    void enter(const char* name);

    /// step up one level
    void leave();

    /// step up multiple n levels in one call
    void leave(int n);

    /// return a reference to the root region
    region_type& regions() { return root_region_; }

    /// return a pointer to the current region
    region_type* current_region() { return current_region_; }

    /// return if in the root region (i.e. the highest level)
    bool is_in_root() const { return &root_region_ == current_region_; }

    /// return if the profiler has been activated
    bool is_activated() const { return activated_; }

    /// start (activate) the profiler
    void start();

    /// stop (deactivate) the profiler
    void stop();

    /// restart the profiler
    /// remove all trace information and restart timer for the root region
    void restart();

    /// the time stamp at which the profiler was started (avtivated)
    timer_type::time_point start_time() const { return start_time_; }

    /// the time stamp at which the profiler was stopped (deavtivated)
    timer_type::time_point stop_time()  const { return stop_time_; }

    /// the time in seconds between activation and deactivation of the profiler
    double wall_time() const {
        return timer_type::difference(start_time_, stop_time_);
    }

    /// stop the profiler then generate the performance tree ready for output
    profiler_node performance_tree();

private:
    void activate()   { activated_ = true;  }
    void deactivate() { activated_ = false; }

    timer_type::time_point start_time_;
    timer_type::time_point stop_time_;
    bool activated_ = false;
    region_type root_region_;
    region_type* current_region_ = &root_region_;
};

#ifdef WITH_PROFILING
namespace data {
    using profiler_wrapper = nest::mc::threading::enumerable_thread_specific<profiler>;
    extern profiler_wrapper profilers_;
}
#endif

/// get a reference to the thread private profiler
/// will lazily create and start the profiler it it has not already been done so
profiler& get_profiler();

/// start thread private profiler
void profiler_start();

/// stop thread private profiler
void profiler_stop();

/// enter a profiling region with name n
void profiler_enter(const char* n);

/// enter nested profiler regions in a single call
template <class...Args>
void profiler_enter(const char* n, Args... args) {
#ifdef WITH_PROFILING
    get_profiler().enter(n);
    profiler_enter(args...);
#endif
}

/// move up one level in the profiler
void profiler_leave();

/// move up multiple profiler levels in one call
void profiler_leave(int nlevels);

/// iterate and stop them
void profilers_stop();

/// reset profilers
void profilers_restart();

/// print the collated profiler to std::cout
void profiler_output(double threshold, std::size_t num_local_work_items);

} // namespace util
} // namespace mc
} // namespace nest

// define some helper macros to make instrumentation of the source code with calls
// to the profiler a little less visually distracting
#define PE nest::mc::util::profiler_enter
#define PL nest::mc::util::profiler_leave
