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


#endif
