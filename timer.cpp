#include "timer.h"
#include <SDL2/SDL.h>

Timer::Timer() {
  StartTicks = 0;
  PauseTicks = 0;
  paused = false;
  started = false;
}

void Timer::start() {
  started = true;
  paused = false;

  StartTicks = SDL_GetTicks();
  PauseTicks = 0;
}

void Timer::stop() {
  started = false;
  paused = false;

  StartTicks = 0;
  PauseTicks = 0;
}

void Timer::pause() {
  if (started && !paused) {
    paused = true;

    PauseTicks = SDL_GetTicks() - StartTicks;
    StartTicks = 0;
  }
}

void Timer::unpause() {
  if (started && paused) {
    paused = false;

    StartTicks = SDL_GetTicks() - PauseTicks;
    PauseTicks = 0;
  }
}

Uint32 Timer::getTicks() {
  Uint32 time = 0;
  if (started) {
    if (paused) {
      time = PauseTicks;
    } else {
      time = SDL_GetTicks() - StartTicks;
    }
  }
  return time;
}

bool Timer::isStarted() {
  return started;
}

bool Timer::isPaused() {
  return paused && started;
}