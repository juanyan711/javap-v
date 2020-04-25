#include "stdafx.h"
#include "util.h"

//下面的都是按照大端序来读取整形数据

uint16_t Util::readUnsignedInt16(const char* b, uint32_t index)
{
	return (uint16_t)(((b[index] & 0xFF) << 8) | (b[index + 1] & 0xFF));
}

uint32_t Util::readUnsignedInt32(const char* b, uint32_t index)
{
	return ((b[index] & 0xFF) << 24) | ((b[index + 1] & 0xFF) << 16)
		| ((b[index + 2] & 0xFF) << 8) | (b[index + 3] & 0xFF);
}

int16_t Util::readInt16(const char* b, uint32_t index)
{
	return (int16_t)(((b[index] & 0xFF) << 8) | (b[index + 1] & 0xFF));
}

int32_t Util::readInt32(const char* b, uint32_t offset)
{
	return readUnsignedInt32(b, offset);
}

QString Util::baseTypeName(const QChar& c)
{
	const QChar upper = c.toUpper();
	QString temp;
	switch (upper.toLatin1()) {
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

QString Util::javaClassName(QString constantName)
{
	bool isArray = false;
	QString javaName;
	if (constantName.startsWith("[")) {
		isArray = true;
		constantName = constantName.mid(1, constantName.size() - 1);
	}
	if (constantName.startsWith("L")) {
		javaName = constantName.mid(1, constantName.size() - 2).replace("/", ".");
	}
	else {
		javaName = Util::baseTypeName(constantName.at(0));
	}

	if (isArray) {
		isArray = false;
		javaName += "[]";
	}
	return javaName;
}



const std::array<uint16_t, 4> AccessFlags::classModifiers = {
		ACC_PUBLIC, ACC_FINAL, ACC_ABSTRACT, ACC_MODULE
};

const std::array<uint16_t, 9> AccessFlags::classFlags = {
ACC_PUBLIC, ACC_FINAL, ACC_SUPER, ACC_INTERFACE, ACC_ABSTRACT,
ACC_SYNTHETIC, ACC_ANNOTATION, ACC_ENUM, ACC_MODULE
};

const std::array<uint16_t, 7> AccessFlags::innerClassModifiers = {
ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL,
ACC_ABSTRACT, ACC_MODULE
};

const std::array<uint16_t, 12> AccessFlags::innerClassFlags = {
ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL, ACC_SUPER,
ACC_INTERFACE, ACC_ABSTRACT, ACC_SYNTHETIC, ACC_ANNOTATION, ACC_ENUM, ACC_MODULE
};

const std::array<uint16_t, 8> AccessFlags::fieldModifiers = {
ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL,
ACC_VOLATILE, ACC_TRANSIENT, ACC_MODULE
};

const std::array<uint16_t, 10> AccessFlags::fieldFlags = {
ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL,
ACC_VOLATILE, ACC_TRANSIENT, ACC_SYNTHETIC, ACC_ENUM, ACC_MODULE
};

const std::array<uint16_t, 10> AccessFlags::methodModifiers = {
ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL,
ACC_SYNCHRONIZED, ACC_NATIVE, ACC_ABSTRACT, ACC_STRICT, ACC_MODULE
};

const std::array<uint16_t, 13> AccessFlags::methodFlags = {
	ACC_PUBLIC, ACC_PRIVATE, ACC_PROTECTED, ACC_STATIC, ACC_FINAL,
	ACC_SYNCHRONIZED, ACC_BRIDGE, ACC_VARARGS, ACC_NATIVE, ACC_ABSTRACT,
	ACC_STRICT, ACC_SYNTHETIC, ACC_MODULE
};

QString AccessFlags::flagToModifier(uint16_t flag, Kind t) {
	switch (flag) {
	case ACC_PUBLIC:
		return "public";
	case ACC_PRIVATE:
		return "private";
	case ACC_PROTECTED:
		return "protected";
	case ACC_STATIC:
		return "static";
	case ACC_FINAL:
		return "final";
	case ACC_SYNCHRONIZED:
		return "synchronized";
	case 0x80:
		return (t == Kind::Field ? "transient" : QString());
	case ACC_VOLATILE:
		return "volatile";
	case ACC_NATIVE:
		return "native";
	case ACC_ABSTRACT:
		return "abstract";
	case ACC_STRICT:
		return "strictfp";
	case ACC_MODULE:
		return "module";
	default:
		return QString();
	}
}

QString AccessFlags::flagToName(uint16_t flag, Kind t) {
	switch (flag) {
	case ACC_PUBLIC:
		return "ACC_PUBLIC";
	case ACC_PRIVATE:
		return "ACC_PRIVATE";
	case ACC_PROTECTED:
		return "ACC_PROTECTED";
	case ACC_STATIC:
		return "ACC_STATIC";
	case ACC_FINAL:
		return "ACC_FINAL";
	case 0x20:
		return (t == Kind::Class ? "ACC_SUPER" : "ACC_SYNCHRONIZED");
	case 0x40:
		return (t == Kind::Field ? "ACC_VOLATILE" : "ACC_BRIDGE");
	case 0x80:
		return (t == Kind::Field ? "ACC_TRANSIENT" : "ACC_VARARGS");
	case ACC_NATIVE:
		return "ACC_NATIVE";
	case ACC_INTERFACE:
		return "ACC_INTERFACE";
	case ACC_ABSTRACT:
		return "ACC_ABSTRACT";
	case ACC_STRICT:
		return "ACC_STRICT";
	case ACC_SYNTHETIC:
		return "ACC_SYNTHETIC";
	case ACC_ANNOTATION:
		return "ACC_ANNOTATION";
	case ACC_ENUM:
		return "ACC_ENUM";
	case ACC_MODULE:
		return "ACC_MODULE";
	default:
		return QString();
	}
}

std::set<QString> AccessFlags::getClassModifiers()
{
	int f = ((flags & ACC_INTERFACE) != 0 ? flags & ~ACC_ABSTRACT : flags);
	return getModifiers(f, classModifiers, Kind::Class);
}

std::set<QString> AccessFlags::getClassFlags()
{
	return getFlags(classFlags, Kind::Class);
}

std::set<QString> AccessFlags::getInnerClassModifiers()
{
	int f = ((flags & ACC_INTERFACE) != 0 ? flags & ~ACC_ABSTRACT : flags);
	return getModifiers(f, innerClassModifiers, Kind::InnerClass);
}

std::set<QString> AccessFlags::getInnerClassFlags()
{
	return getFlags(innerClassFlags, Kind::InnerClass);
}

std::set<QString> AccessFlags::getFieldModifiers()
{
	return getModifiers(fieldModifiers, Kind::Field);
}

std::set<QString> AccessFlags::getFieldFlags()
{
	return getFlags(fieldFlags, Kind::Field);
}

std::set<QString> AccessFlags::getMethodModifiers()
{
	return getModifiers(methodModifiers, Kind::Method);
}

std::set<QString> AccessFlags::getMethodFlags()
{
	return getFlags(methodFlags, Kind::Method);
}

AccessFlags::AccessFlags(uint16_t fs) :flags(fs)
{
}

AccessFlags AccessFlags::ignore(int mask)
{
	return AccessFlags(this->flags & ~mask);
}

bool AccessFlags::is(int mask)
{
	return (flags & mask) != 0;
}


