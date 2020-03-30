#pragma once

#include<cstdint>

class Util
{
public:
	static uint16_t readUnsignedInt16(const char*, uint32_t );
	static uint32_t readUnsignedInt32(const char*, uint32_t );

	template<typename T>
	static uint32_t digits(T v) {
		uint32_t len = 1;
		if (v < 0) {
			v = 0 - v;
		}
		while (true) {
			v = v / 10;
			if (v != 0) {
				len++;
			}
			else {
				break;
			}
		}
		return len;
	}

	static QString baseTypeName(QChar& c);
};


