#ifndef TIMER_H
#define TIMER_H

class Timer {
public:
	Timer();
	Timer(const double time);
	Timer(const Timer& rhs);

	// time interval
	void set(const double time);
	void pause();
	void reset();
	void offset(const double time);

	// returns true once _time has passed and restarts
	bool update();

	bool get_pause();
	double get_time();
	double get_clock();
private:
	double _time;
	double _prev_time;
	double _clock;
	bool _is_paused;
};

#endif