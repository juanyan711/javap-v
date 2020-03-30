#include "stdafx.h"
#include "util.h"

//下面的都是按照大端序来读取整形数据

uint16_t Util::readUnsignedInt16(const char* b, uint32_t index)
{
	return (int16_t)(((b[index] & 0xFF) << 8) | (b[index + 1] & 0xFF));
}

uint32_t Util::readUnsignedInt32(const char* b, uint32_t index)
{
	return ((b[index] & 0xFF) << 24) | ((b[index + 1] & 0xFF) << 16)
		| ((b[index + 2] & 0xFF) << 8) | (b[index + 3] & 0xFF);
}

QString Util::baseTypeName(QChar& c)
{
	c = c.toUpper();
	QString temp;
	switch (c.toLatin1()) {
	case 'Z':
		temp = "boolean";
		break;
	case 'C':
		temp = "char";
		break;
	case 'B':
		temp = "byte";
		break;
	case 'S':
		temp = "short";
		break;
	case 'I':
		temp = "int";
		break;
	case 'F':
		temp = "float";
		break;
	case 'J':
		temp = "long";
		break;
	case 'D':
		temp = "double";
		break;
	case 'V':
		temp = "void";
		break;
	default:
		temp = "unknown";
	}
	return temp;
}


