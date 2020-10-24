#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string_view>

#define STR_PADDING 54

class PacketData {
public:
	PacketData(const char* key);
	PacketData(PacketData&& rhs) noexcept;

	template<typename ... Args>
	PacketData(Args ... args);

	template<typename First, typename ...Rest>
	void add(const First& first, const Rest& ... rest);

	void add(int data);
	void add(float data);
	void add(double data);
	void add(const char* data);
	void add(std::string data);
	
	template <class T>
	void add(T data);

	void add(PacketData&& rhs);

	const char* c_str();
	int length();
private:
	std::vector<uint8_t> _data;
};

template<typename ... Args>
PacketData::PacketData(Args ... args)
{
	_data.resize(4);
	int header = 4;
	memcpy(&_data[0], &header, sizeof(int));

	add(args...);
}

template<typename First, typename ... Rest>
void PacketData::add(const First& first, const Rest& ... rest) {
	add(first);
	add(rest...);
}

template <class T>
void PacketData::add(T data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

#endif