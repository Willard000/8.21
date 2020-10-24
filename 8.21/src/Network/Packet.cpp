#include "Packet.h"

#include <cassert>
#include <iterator>

PacketData::PacketData(const char* key) {
	_data.resize(4);
	int header = 4;
	memcpy(&_data[0], &header, sizeof(int));

	add(key);
}

PacketData::PacketData(PacketData&& rhs) noexcept :
	_data(std::move(rhs._data))
{}

void PacketData::add(int data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

void PacketData::add(float data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

void PacketData::add(double data) {
	uint8_t* ptr = static_cast<uint8_t*>((static_cast<void*>(&data)));
	std::copy(ptr, ptr + sizeof(data), std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

void PacketData::add(const char* data) {
	std::copy(data, data + strlen(data) + 1, std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

void PacketData::add(std::string data) {
	assert(false); // use c_str()
	assert(data.size() <= STR_PADDING);

	char buf[STR_PADDING];
	memset(&buf, 0, STR_PADDING);
	memcpy(&buf, data.data(), data.size());

	std::copy(buf, buf + STR_PADDING, std::back_inserter(_data));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

void PacketData::add(PacketData&& rhs) {
	_data.insert(_data.end(), std::make_move_iterator(rhs._data.begin() + 4), std::make_move_iterator(rhs._data.end()));

	int size = _data.size();
	memcpy(&_data[0], &size, sizeof(int));
}

int PacketData::length() {
	return _data.size();
}

const char* PacketData::c_str() {
	return static_cast<const char*>(static_cast<void*>(&_data[0]));
}
