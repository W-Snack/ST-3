// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class MockTimer : public Timer {
 public:
  MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    timedDoor = new TimedDoor(5);
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete timedDoor;
    delete mockClient;
  }

  TimedDoor* timedDoor;
  MockTimerClient* mockClient;
};

TEST_F(TimedDoorTest, InitialStateIsLocked) {
  ASSERT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
  timedDoor->unlock();
  ASSERT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  timedDoor->unlock();
  timedDoor->lock();
  ASSERT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, TimerRegistration) {
  testing::NiceMock<MockTimerClient> client;
  Timer timer;
  EXPECT_CALL(client, Timeout()).Times(1);
  timer.tregister(1, &client);
}

TEST_F(TimedDoorTest, ThrowsWhenDoorRemainsOpen) {
  timedDoor->unlock();
  ASSERT_THROW(timedDoor->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowWhenDoorClosed) {
  timedDoor->lock();
  ASSERT_NO_THROW(timedDoor->throwState());
}

TEST_F(TimedDoorTest, TimeoutValueIsCorrect) {
  ASSERT_EQ(timedDoor->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, AdapterCallsTimeout) {
  DoorTimerAdapter adapter(*timedDoor);
  EXPECT_CALL(*mockClient, Timeout()).Times(0);
  adapter.Timeout();
}

TEST_F(TimedDoorTest, AdapterTriggersDoorStateCheck) {
  DoorTimerAdapter adapter(*timedDoor);
  timedDoor->unlock();
  ASSERT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, FullTimerScenario) {
  testing::NiceMock<MockTimerClient> client;
  Timer timer;
  timedDoor->unlock();

  std::future<void> future =
      timer.tregister(timedDoor->getTimeOut(), timedDoor->getAdapter());

  future.wait();

  ASSERT_THROW(future.get(), std::runtime_error);
}
