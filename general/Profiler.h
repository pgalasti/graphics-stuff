#ifndef G_STUFF_PROFILER_H
#define G_STUFF_PROFILER_H

#include <chrono>
#include <iostream>
#include <string>
#include <string_view>

namespace GStuff::General {

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

template <typename TimeUnit>
class Profiler {
public:
  Profiler() = default;

  struct Snapshot {
    std::string tag;
    long int elapsedTime;

    friend std::ostream& operator<<(std::ostream& os, const Snapshot snapshot) {
      const auto [tag, time] = snapshot;
      os << "Tag: " << tag << ": " << time << "ms\n";
      return os;
    }
  };

  void Start() {
    Start(m_LastTag);
  }

  void Start(std::string_view tag) {
    m_LastTag = tag;
    m_StartTimePoint = std::chrono::steady_clock::now();
  }

  Snapshot Stop() {
    const TimePoint end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<TimeUnit>(end - m_StartTimePoint).count();
    
    return Snapshot{
      m_LastTag,
      elapsed
    };   
  }

private:
  TimePoint m_StartTimePoint;
  std::string m_LastTag {};
};

using ProfilerMs = Profiler<std::chrono::milliseconds>;

}

#define PROF_CONCAT_INNER(a, b) a##b
#define PROF_CONCAT(a, b)       PROF_CONCAT_INNER(a, b)

#ifdef __COUNTER__
    #define PROF_UNIQUE_NAME(base) PROF_CONCAT(base, __COUNTER__)
#else
    #define PROF_UNIQUE_NAME(base) PROF_CONCAT(base, __LINE__)
#endif

#ifdef PROFILER_ENABLE_INLINE_MACRO

    #define STDOUT_PROFILE_FUNC_MS_IMPL(prof, tagVar, tag, ...)       \
        do {                                                          \
            const char* tagVar = (tag);                               \
            GStuff::General::ProfilerMs prof;                         \
            prof.Start(tagVar);                                       \
            (__VA_ARGS__);                                            \
            std::cout << prof.Stop();                                 \
        } while (0)

    #define STDOUT_PROFILE_FUNC_MS(tag, ...)                          \
        STDOUT_PROFILE_FUNC_MS_IMPL(PROF_UNIQUE_NAME(prof_),          \
                                    PROF_UNIQUE_NAME(tag_),           \
                                    tag, __VA_ARGS__)

#else

    #define STDOUT_PROFILE_FUNC_MS(tag, ...) \
        do { (__VA_ARGS__); } while (0)

#endif // PROFILER_ENABLE_INLINE_MACRO
       
#endif
