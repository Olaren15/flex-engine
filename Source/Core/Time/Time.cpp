#include "Time.hpp"

#include <SDL2/SDL.h>

namespace cbl {

uint32_t Time::mLastTick = 0;
float Time::mDeltaSeconds = 0.0f;

void Time::tick() {
  uint32_t tickTime = SDL_GetTicks();
  mDeltaSeconds = static_cast<float>(tickTime - mLastTick) / 1000;
  mLastTick = tickTime;
}

float Time::deltaSeconds() { return mDeltaSeconds; }
} // namespace flex