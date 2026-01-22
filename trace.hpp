#include <vector>
#include <chrono>
#include <fstream>
#include <mutex>
#include <iostream>

// 1. Define the event structure
struct TraceEvent {
    const char* name;
    const char* cat;
    uint64_t tid;
    char phase; // 'B' for Begin, 'E' for End
    uint64_t ts; // Timestamp in microseconds
};

// 2. Thread-local storage to avoid locks during the hot loop
// Reserve enough space to avoid re-allocation during simulation!
static thread_local std::vector<TraceEvent> *t_events; 

void init_profiler() {
    std::cout << "init profiler\n";
    (*t_events).reserve(100); // Adjust based on your loop count
    std::cout << "init profiler\n";
}

// High-res timer helper
inline uint64_t get_time_us() {
    using namespace std::chrono;
    return duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

// 3. Instrumentation Macros
#define TRACE_BEGIN(vec, name) \
    (vec)->push_back({name, "sim", tid, 'B', get_time_us()})

#define TRACE_END(vec, name) \
    (vec)->push_back({name, "sim", tid, 'E', get_time_us()})

inline void dump_chrome_trace(const std::string& filename,
                              const std::vector<std::vector<TraceEvent>>& all_threads_data) {
    std::ofstream out(filename);
    out << "[";
    bool first = true;

    for (const auto& thread_vec : all_threads_data) {
        for (const auto& ev : thread_vec) {
            if (!first) out << ",";
            out << "\n    {"
                << "\"name\": \"" << ev.name << "\", "
                << "\"cat\": \"PERF\", "
                << "\"ph\": \"" << ev.phase << "\", "
                << "\"pid\": 1, "
                << "\"tid\": " << ev.tid << ", "
                << "\"ts\": " << ev.ts
                << "}";
            first = false;
        }
    }
    out << "\n]";
    out.close();
    std::cout << "[Profiler] Trace written to " << filename << std::endl;
}
