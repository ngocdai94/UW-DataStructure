// time.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "time.h"

#include <iomanip>
#include <sstream>

using namespace std;


const unsigned int TimeDiff::HOUR = 60;
const unsigned int TimeDiff::DAY = 24 * 60;

const TimePoint TimePoint::min_time = TimePoint(0);
const TimePoint TimePoint::max_time = TimePoint(3651159);


TimeDiff::TimeDiff(const TimePoint& time1, const TimePoint& time2) {
  minutes_ = time2.minutes_ - time1.minutes_;
}


TimeDiff TimeDiff::operator+(const TimeDiff& interval2) const {
  TimeDiff result = *this;
  result.minutes_ += interval2.minutes_;
  return result;
}


void TimeDiff::dump(ostream& out, int depth) const {
  string indent(4 * depth, ' ' );
  unsigned d = minutes_ / DAY;
  unsigned h = (minutes_ - d * DAY) / HOUR;
  unsigned m = minutes_ - (d * DAY + h * HOUR);
  out << indent << minutes_ << " minutes ("
      << d << " days, "
      << h << " hours, "
      << m << " minutes)"
      << endl;
}


TimePoint::TimePoint(unsigned int dddhhmm) {
  unsigned int d = dddhhmm / 10000;
  unsigned int h = (dddhhmm / 100) % 100;
  unsigned int m = dddhhmm % 100;

  minutes_ = ((d * 24) + h) * 60 + m;
}


TimePoint TimePoint::operator+(const TimeDiff& delta) const {
  TimePoint result = *this;
  result.minutes_ += delta.minutes_;
  return result;
}


string TimePoint::repr() const {
  unsigned d = minutes_ / TimeDiff::DAY;
  unsigned h = (minutes_ - d * TimeDiff::DAY) / TimeDiff::HOUR;
  unsigned m = minutes_ - (d * TimeDiff::DAY + h * TimeDiff::HOUR);
  ostringstream out;
  out << "day " << d
      << " at " << setw(2) << h
      << ":" << setw(2) << setfill('0')<< m;
  return out.str();
}


void TimePoint::dump(ostream& out, int depth) const {
  string indent(4 * depth, ' ' );
  out << indent << repr() << endl;
}


int time_compare(const TimePoint& time1, const TimePoint& time2) {
  if (time1.minutes_ < time2.minutes_) {
    return -1;
  }
  if (time1.minutes_ > time2.minutes_) {
    return 1;
  }
  return 0;
}


bool operator<(const TimePoint& time1, const TimePoint& time2) {
  return time_compare(time1, time2) < 0;
}


ostream& operator<<(ostream& out, const TimePoint& time_point) {
  if (time_point.minutes_ == 0) {
    return out << "EPOCH";
  }

  if (time_point == TimePoint::max_time) {
    return out << "MAX_TIME";
  }

  unsigned d = time_point.minutes_ / TimeDiff::DAY;
  unsigned h = (time_point.minutes_ - d * TimeDiff::DAY) / TimeDiff::HOUR;
  unsigned m = time_point.minutes_ - (d * TimeDiff::DAY + h * TimeDiff::HOUR);

#if 0
  return out << d * 10000 + h * 100 + m;
#else
  return out << "day " << d
             << " at " <<  setw(2) << h
             << ":" << setw(2) << setfill('0') << m << setfill(' ');
#endif
}
