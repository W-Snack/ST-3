// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_
#include <future>

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
 public:
  virtual void Timeout() = 0;
};

class Door {
 public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
};

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;

 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout();
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  int iTimeout;
  bool isOpened;

 public:
  explicit TimedDoor(int);
  bool isDoorOpened();
  void unlock();
  void lock();
  int getTimeOut() const;
  void throwState();
  DoorTimerAdapter* getAdapter() const;
};

class Timer {
  TimerClient* client;
  std::future<void> timerFuture;
  void sleep(int seconds);

 public:
  virtual ~Timer() = default;
  virtual std::future<void> tregister(int, TimerClient*);
};
#endif  // INCLUDE_TIMEDDOOR_H_
