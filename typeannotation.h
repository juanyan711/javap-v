#pragma once
#include "constantPool.h"

#include<memory>
#include<vector>
#include<array>

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::array;

namespace jdk{



//固定三个字节大小
class TypePathEntry {
public:
	enum class TypePathEntryKind:uint8_t {
		ARRAY = 0,
		INNER_TYPE,
		WILDCARD,
		TYPE_ARGUMENT
	};

	TypePathEntry(TypePathEntryKind tag, int arg);
private:
	TypePathEntry(TypePathEntryKind tag);

public:
	static const int bytesPerEntry = 2;   //每个TypePathEntry的字节大小
	static shared_ptr<TypePathEntry> fromBinary(uint8_t tag, uint8_t arg);

	const TypePathEntryKind tag;    //参数一
	const uint8_t arg;

	static const shared_ptr<TypePathEntry> ARRAY;
	static const shared_ptr<TypePathEntry> INNER_TYPE;
	static const shared_ptr<TypePathEntry> WILDCARD;

	QString toString() const;
};

class TargetType {
private:
	TargetType(int targetTypeValue);
	TargetType(int targetTypeValue, bool isLocal);

private:
	static const int MAXIMUM_TARGET_TYPE_VALUE = 0x4B;
	

private:
	int targetTypeValue;
    bool local;

public:
	bool isLocal();

	int getTargetTypeValue();

	static bool isValidTargetTypeValue(int tag);

	QString toString();
	static shared_ptr<TargetType> fromTargetTypeValue(int tag);
public:

	enum Type {
		CLASS_TYPE_PARAMETER = 0x00,
		METHOD_TYPE_PARAMETER = 0x01,
		CLASS_EXTENDS = 0x10,
		CLASS_TYPE_PARAMETER_BOUND = 0x11,
		METHOD_TYPE_PARAMETER_BOUND = 0x12,
		FIELD = 0x13,
		METHOD_RETURN = 0x14,
		METHOD_RECEIVER = 0x15,
		METHOD_FORMAL_PARAMETER = 0x16,
		THROWS = 0x17,
		LOCAL_VARIABLE = 0x40,
		RESOURCE_VARIABLE = 0x41,
		EXCEPTION_PARAMETER = 0x42,
		INSTANCEOF = 0x43,
		NEW = 0x44,
		CONSTRUCTOR_REFERENCE = 0x45,
		METHOD_REFERENCE = 0x46,
		CAST = 0x47,
		CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT = 0x48,
		METHOD_INVOCATION_TYPE_ARGUMENT = 0x49,
		CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT = 0x4A,
		METHOD_REFERENCE_TYPE_ARGUMENT = 0x4B,
		UNKNOWN = 0xFF
	};
};

class Position {
public:
	Position();

	QString toString();
	bool emitToClassfile();

	static shared_ptr<Position> read_position(const char* cr, uint32_t& length);
public:
	shared_ptr<TargetType> type ;
	vector<shared_ptr<TypePathEntry>> location;
	//int pos = -1;     // Tree position.
	boolean isValidOffset = false;
	int offset = -1;

	vector<int> lvarOffset;
	vector<int> lvarLength;
	vector<int> lvarIndex;

	int bound_index = 0x7FFFFFFF;
	int parameter_index = 0x80000000;
	int type_index = 0x80000000;
	int exception_index = 0x80000000;

};


class TypeAnnotation {

};

}

