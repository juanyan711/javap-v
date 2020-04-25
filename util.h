#pragma once

#include<cstdint>
#include<set>
#include<array>
#include<QChar>

using std::set;
using std::array;

class Util
{
public:
	static uint16_t readUnsignedInt16(const char*, uint32_t );
	static uint32_t readUnsignedInt32(const char*, uint32_t );

	static int16_t readInt16(const char*, uint32_t);
	static int32_t readInt32(const char*, uint32_t);

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

	static QString baseTypeName(const QChar& c);
	static QString javaClassName(QString constantName);
};

class AccessFlags {
public:
	static const uint16_t ACC_PUBLIC = 0x0001;         // class, inner, field, method
	static const uint16_t ACC_PRIVATE = 0x0002;        //        inner, field, method
	static const uint16_t ACC_PROTECTED = 0x0004;      //        inner, field, method
	static const uint16_t ACC_STATIC = 0x0008;         //        inner, field, method
	static const uint16_t ACC_FINAL = 0x0010;          // class, inner, field, method
	static const uint16_t ACC_SUPER = 0x0020;          // class
	static const uint16_t ACC_SYNCHRONIZED = 0x0020;   //                      method
	static const uint16_t ACC_VOLATILE = 0x0040;       //               field
	static const uint16_t ACC_BRIDGE = 0x0040;         //                      method
	static const uint16_t ACC_TRANSIENT = 0x0080;      //               field
	static const uint16_t ACC_VARARGS = 0x0080;        //                      method
	static const uint16_t ACC_NATIVE = 0x0100;         //                      method
	static const uint16_t ACC_INTERFACE = 0x0200;      // class, inner
	static const uint16_t ACC_ABSTRACT = 0x0400;       // class, inner,        method
	static const uint16_t ACC_STRICT = 0x0800;         //                      method
	static const uint16_t ACC_SYNTHETIC = 0x1000;      // class, inner, field, method
	static const uint16_t ACC_ANNOTATION = 0x2000;     // class, inner
	static const uint16_t ACC_ENUM = 0x4000;           // class, inner, field
	static const uint16_t ACC_MODULE = 0x8000;         // class, inner, field, method
	enum Kind { Class, InnerClass, Field, Method };

private:
	static const std::array<uint16_t, 4> classModifiers;
	static const std::array<uint16_t, 9> classFlags;
	static const std::array<uint16_t, 7> innerClassModifiers;
	static const std::array<uint16_t, 12> innerClassFlags;
	static const std::array<uint16_t, 8> fieldModifiers;
	static const std::array<uint16_t, 10> fieldFlags;
	static const std::array<uint16_t, 10> methodModifiers;
	static const std::array<uint16_t, 13> methodFlags;

	static QString flagToModifier(uint16_t flag, Kind t);
	static QString flagToName(uint16_t flag, Kind t);

public:
	std::set<QString> getClassModifiers();

	std::set<QString> getClassFlags();

	std::set<QString> getInnerClassModifiers();

	std::set<QString> getInnerClassFlags();

	std::set<QString> getFieldModifiers();

	std::set<QString> getFieldFlags();

	std::set<QString> getMethodModifiers();

	std::set<QString> getMethodFlags();

	template<uint32_t N>
	std::set<QString> getModifiers(std::array<uint16_t, N> modifierFlags, Kind t) {
		return getModifiers<N>(flags, modifierFlags, t);
	}

private:
	template<uint32_t N>
	static std::set<QString> getModifiers(uint16_t flags, std::array<uint16_t, N> modifierFlags, Kind t) {
		std::set<QString> s;
		for (auto m : modifierFlags) {
			if ((flags & m) != 0)
				s.emplace(flagToModifier(m, t));
		}
		return std::move(s);
	}

	template<uint32_t N>
	std::set<QString> getFlags(std::array<uint16_t, N> expectedFlags, Kind t) {
		std::set<QString> s;
		int f = flags;
		for (auto e : expectedFlags) {
			if ((f & e) != 0) {
				s.emplace(flagToName(e, t));
				f = f & ~e;
			}
		}
		/*	while (f != 0) {
				int bit = Integer.highestOneBit(f);
				s.emplace("0x" + Integer.toHexString(bit));
				f = f & ~bit;
			}*/
		return std::move(s);
	}

public:
	AccessFlags(uint16_t fs);
	AccessFlags ignore(int mask);
	bool is(int mask);
private:
	const uint16_t flags;
};




