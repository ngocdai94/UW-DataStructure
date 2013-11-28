// time.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_TIME_H_)
#define CSS343_TIME_H_

#include <iostream>
#include <ostream>
#include <string>


class TimePoint;


class TimeDiff {
public:
  static const unsigned int DAY;
  static const unsigned int HOUR;

  TimeDiff(const unsigned int minutes) : minutes_(minutes) {}
  TimeDiff(const TimePoint& time1, const TimePoint& time2);

  TimeDiff operator+(const TimeDiff& interval) const;

  void dump(std::ostream& out=std::cerr, int depth=0) const;

  friend class TimePoint;

private:
  unsigned int minutes_;
};


class TimePoint {
public:
  static const TimePoint max_time;
  static const TimePoint min_time;

  TimePoint(unsigned int dddhhmm = 0);

  bool operator==(const TimePoint& rhs) const {
    return minutes_ == rhs.minutes_;
  }

  TimePoint operator+(const TimeDiff& delta) const;

  std::string repr() const;

  void dump(std::ostream& out=std::cerr, int depth=0) const;

  friend class TimeDiff;

  friend std::ostream& operator<<(std::ostream& out, const TimePoint& time_point);

private:
  friend int time_compare(const TimePoint& time1, const TimePoint& time2);

  unsigned int minutes_;
};


int time_compare(const TimePoint& time1, const TimePoint& time2);

bool operator<(const TimePoint& time1, const TimePoint& time2);

std::ostream& operator<<(std::ostream& out, const TimePoint& time_point);


#endif
