#ifndef GSTUFF_GENERAL_H
#define GSTUFF_GENERAL_H

namespace GStuff::General {

template <typename T >
struct FrameStat {
  T DeltaTime{};
  T LastFrame{};
  void update(const T& currentFrame) {DeltaTime = currentFrame - LastFrame; LastFrame = currentFrame;}
};

using FrameStatf = FrameStat<float>;

}

#endif 
