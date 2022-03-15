#pragma once
#include <SDL2/SDL.h>
class Timer {
public:
  Timer();

  //funzioni per start, stop e reset
  void start();
  void stop();
  void pause();
  void unpause();

  Uint32 getTicks();

  bool isStarted();
  bool isPaused();

private:
  // ticks all'avvio del timer
  Uint32 StartTicks;
  Uint32 PauseTicks;
  // stato del timer
  bool paused;
  bool started;
};