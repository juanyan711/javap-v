#include "stdafx.h"
#include "classFile.h"

using namespace jdk;

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

std::set<QString> jdk::AccessFlags::getClassModifiers()
{
	int f = ((flags & ACC_INTERFACE) != 0 ? flags & ~ACC_ABSTRACT : flags);
	return getModifiers(f, classModifiers, Kind::Class);
}

std::set<QString> jdk::AccessFlags::getClassFlags()
{
	return getFlags(classFlags, Kind::Class);
}

std::set<QString> jdk::AccessFlags::getInnerClassModifiers()
{
	int f = ((flags & ACC_INTERFACE) != 0 ? flags & ~ACC_ABSTRACT : flags);
	return getModifiers(f, innerClassModifiers, Kind::InnerClass);
}

std::set<QString> jdk::AccessFlags::getInnerClassFlags()
{
	return getFlags(innerClassFlags, Kind::InnerClass);
}

std::set<QString> jdk::AccessFlags::getFieldModifiers()
{
	return getModifiers(fieldModifiers, Kind::Field);
}

std::set<QString> jdk::AccessFlags::getFieldFlags()
{
	return getFlags(fieldFlags, Kind::Field);
}

std::set<QString> jdk::AccessFlags::getMethodModifiers()
{
	return getModifiers(methodModifiers, Kind::Method);
}

std::set<QString> jdk::AccessFlags::getMethodFlags()
{
	return getFlags(methodFlags, Kind::Method);
}

jdk::ClassFile::ClassFile(const QString& fn, const QByteArray& s, const uint32_t offset):fileName(fn), src(s)
{
	const char* bytes = src.constData();
	magic = Util::readUnsignedInt32(bytes, offset);
	if (magic != 0xCAFEBABE) {
		throw JdkException("magic value is fault", FILE_INFO);
	}
	minor_version = Util::readUnsignedInt16(bytes, offset + 4);
	major_version = Util::readUnsignedInt16(bytes, offset + 6);
	if (!ClsVersion::check(minor_version << 16 | major_version)) {
		throw JdkException("class version max is jdk1.8", FILE_INFO);
	}
	pool = ConstantPool::createConstantPool(src, 8);
	accessFlagsOffset = pool->getLength() + 8 + offset;
	accessFlags = Util::readUnsignedInt16(bytes, accessFlagsOffset);

	thisClassOffset = accessFlagsOffset + 2;
	thisClassIndex = Util::readUnsignedInt16(bytes, thisClassOffset);

	superClassOffset = thisClassOffset + 2;
	superClassIndex = Util::readUnsignedInt16(bytes, superClassOffset);

	interfaceOffset = superClassOffset + 2;
	interfaceCount = Util::readUnsignedInt16(bytes, interfaceOffset);
	if (interfaceCount > 0) {
		for (int i = 0; i < interfaceCount; i++) {
			interfaceIndexs.push_back(Util::readUnsignedInt16(bytes, interfaceOffset + 2 + i * 2));
		}
	}

	uint32_t fieldOffset = interfaceOffset + 2 + interfaceCount * 2;
	fieldCount = Util::readUnsignedInt16(bytes, fieldOffset);
	uint32_t nextFieldOffset = fieldOffset + 2;
	for (int i = 0; i < fieldCount; i++) {
		auto field = FieldInfo::create(bytes, nextFieldOffset, pool.get());
		fields.push_back(field);
		nextFieldOffset += field->getLength();
	}

	uint32_t methodOffset = nextFieldOffset;
	methodCount = Util::readUnsignedInt16(bytes, methodOffset);
	uint32_t nextMethodOffset = methodOffset + 2;
	for (int i = 0; i < methodCount; i++) {
		auto method = FieldInfo::create(bytes, nextMethodOffset, pool.get());
		qDebug() << "method name is " << method->getName();
		methods.push_back(method);
		nextMethodOffset += method->getLength();
	}

	attributeCount = Util::readUnsignedInt16(bytes, nextMethodOffset);
	uint32_t nextAttributeOffset = nextMethodOffset + 2;
	for (int i = 0; i < attributeCount; i++) {
		auto attr = AttributeInfo::createAttributeInfo(bytes, nextAttributeOffset, pool.get());
		attributes.push_back(attr);
		nextAttributeOffset += attr->getBodyLength() + 6;
		auto attrName = attr->getAttributeName();
		if (QString::compare("SourceFile", attrName) == 0) {
			sourceFileIndex = i;
		}
		else if (QString::compare("Signature", attrName)==0) {
			QString sign = SignatureReader::getSignature(*attr, *pool);
			qDebug() << "class signature is:" << sign;
			signatureIndex = i;
		}
		else if (QString::compare("Deprecated", attrName) == 0) {
			deprecatedIndex = i;
		}
	}

}

QString jdk::ClassFile::getFileName() const
{
	return this->fileName;
}

const char* jdk::ClassFile::getBuffer() const
{
	return src.constData();
}

uint32_t jdk::ClassFile::getMagic() const
{
	return magic;
}

uint16_t jdk::ClassFile::getMinor() const
{
	return minor_version;
}

uint16_t jdk::ClassFile::getMajor() const
{
	return major_version;
}

uint16_t jdk::ClassFile::getAccessFlags() const
{
	return accessFlags;
}

uint32_t jdk::ClassFile::getAccessFlagsOffset() const
{
	return accessFlagsOffset;
}

QString jdk::ClassFile::getThisClass() const
{
	return pool->getClassName(thisClassIndex);
}

uint32_t jdk::ClassFile::getThisClassOffset() const
{
	return thisClassOffset;
}

uint16_t jdk::ClassFile::getThisClassIndex() const
{
	return this->thisClassIndex;
}

QString jdk::ClassFile::getSuperClass() const
{
	if (superClassIndex == 0) {
		return ClassInfo::OBJECT;
	}
	return pool->getClassName(superClassIndex);
}

uint32_t jdk::ClassFile::getSuperClassOffset() const
{
	return superClassOffset;
}

uint16_t jdk::ClassFile::getSuperClassIndex() const
{
	return this->superClassIndex;
}

uint32_t jdk::ClassFile::getInterfaceOffset() const
{
	return this->interfaceOffset;
}

const std::vector<QString> jdk::ClassFile::getInterfaces() const
{
	std::vector<QString> result;
	for (auto itr : interfaceIndexs) {
		result.push_back(pool->getClassName(itr));
	}
	return std::move(result);
}

uint16_t jdk::ClassFile::getInterfaceCount() const
{
	if (interfaceCount != interfaceIndexs.size()) {
		qDebug() << "interface count error";
		throw JdkException("inteface count error", FILE_INFO);
	}
	return interfaceCount;
}

uint32_t jdk::ClassFile::getFieldCount() const
{
	return fieldCount;
}

const std::vector<std::shared_ptr<FieldInfo>>& jdk::ClassFile::getFields() const
{
	return fields;
}

uint32_t jdk::ClassFile::getMethodCount() const
{
	return this->methodCount;
}

const std::vector<std::shared_ptr<MethodInfo>>& jdk::ClassFile::getMethods() const
{
	// TODO: 在此处插入 return 语句
	return this->methods;
}

uint32_t jdk::ClassFile::getAttributeCount() const
{
	return this->attributeCount;
}

std::shared_ptr<ConstantPool> jdk::ClassFile::getConstantPool() const
{
	return pool;
}

QString jdk::ClassFile::getSourceFile() const
{
	if (sourceFileIndex == -1) {
		return QString();
	}
	auto attrInfo = attributes[sourceFileIndex];
	uint16_t utf8Index = 0;
	attrInfo->visit([&utf8Index](std::shared_ptr<const AttributeInfo> info) {
		const char* body = info->getBody();
		utf8Index = Util::readUnsignedInt16(body, 0);
		});
	return pool->getUtf8(utf8Index);
}

QString jdk::ClassFile::getSignature() const
{
	if (signatureIndex == -1) {
		return QString();
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignature(*attrInfo, *pool);
}

bool jdk::ClsVersion::check(uint32_t ver)
{
	if (ver != V1_1) {
		if (ver < V1_2 || ver > V12) {
			return false;
		}
	}
	return true;
}

jdk::AccessFlags::AccessFlags(uint16_t fs):flags(fs)
{
}

AccessFlags jdk::AccessFlags::ignore(int mask)
{
	return AccessFlags(this->flags & ~mask);
}

bool jdk::AccessFlags::is(int mask)
{
	return (flags & mask) != 0;
}
