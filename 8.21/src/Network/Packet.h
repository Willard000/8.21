#ifndef PACKET_H
#define PACKET_H

#include <vector>

class PacketData {
public:
	PacketData(const char* key);

	template<typename ... Args>
	PacketData(Args ... args);

	template<typename First, typename ...Rest>
	void add(const First& first, const Rest& ... rest);

	void add(int data);
	void add(float data);
	void add(double data);
	void add(const char* data);

	const char* c_str();
	int length();
private:
	std::vector<uint8_t> _data;
};

template<typename ... Args>
PacketData::PacketData(Args ... args)
{
	add(args...);
}

template<typename First, typename ... Rest>
void PacketData::add(const First& first, const Rest& ... rest) {
	add(first);
	add(rest...);
}

#endif