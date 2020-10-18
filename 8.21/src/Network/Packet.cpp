#include "Packet.h"

#include <cassert>
#include <iterator>

#define VALID_STR_LEN(str) strlen(str) < 54

PacketData::PacketData(const char* key) {
	add(key);
}

void PacketData::add(int data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));
}

void PacketData::add(float data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));
}

void PacketData::add(double data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));
}

void PacketData::add(const char* data) {
	assert(VALID_STR_LEN(data));

	char buf[54];
	memset(&buf, 0, 54);
	memcpy(&buf, data, strlen(data));

	std::copy(buf, buf + 54, std::back_inserter(_data));
}

int PacketData::length() {
	return _data.size();
}

const char* PacketData::c_str() {
	return static_cast<const char*>(static_cast<void*>(&_data[0]));
}
