#include "stdafx.h"
#include "classFile.h"

using namespace jdk;


jdk::ClassFile::ClassFile(const QString& fn, const QByteArray& s, const uint32_t o):fileName(fn), src(s), offset(o)
{
}

void jdk::ClassFile::load(bool check)
{
	const char* bytes = src.constData();
	magic = Util::readUnsignedInt32(bytes, offset);
	if (magic != 0xCAFEBABE) {
		throw JdkException("magic value is fault", FILE_INFO);
	}
	minor_version = Util::readUnsignedInt16(bytes, offset + 4);
	major_version = Util::readUnsignedInt16(bytes, offset + 6);
	if (!ClsVersion::check(minor_version << 16 | major_version)) {
		throw JdkException("class version error", FILE_INFO);
	}
	if (check && major_version>52) {
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
		auto method = MethodInfo::create(bytes, nextMethodOffset, pool.get());
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
		else if (QString::compare("Signature", attrName) == 0) {
			QString sign = SignatureReader::getSignature(*attr, *pool);
			qDebug() << "class signature is:" << sign;
			signatureIndex = i;
		}
		else if (QString::compare("Deprecated", attrName) == 0) {
			deprecatedIndex = i;
		}
	}
}

uint32_t jdk::ClassFile::getOffset() const
{
	return offset;
}

QString jdk::ClassFile::getFileName() const
{
	return this->fileName;
}

const char* jdk::ClassFile::getBuffer() const
{
	return src.constData();
}

uint32_t jdk::ClassFile::getBufferSize() const
{
	return src.size();
}

uint32_t jdk::ClassFile::getMagic() const
{
	return magic;
}

QString jdk::ClassFile::getMagicValue() const
{
	const char* buffer = src.constData();
	return QString::fromLocal8Bit("%1 %2 %3 %4")
		.arg(0x000000FF & *(buffer+offset), 2, 16, QLatin1Char('0'))
		.arg(0x000000FF & *(buffer + offset + 1), 2, 16, QLatin1Char('0'))
		.arg(0x000000FF & *(buffer + offset + 2), 2, 16, QLatin1Char('0'))
		.arg(0x000000FF & *(buffer + offset + 3), 2, 16, QLatin1Char('0'));
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

const std::vector<std::shared_ptr<AttributeInfo>>& jdk::ClassFile::getAttributes() const
{
	// TODO: 在此处插入 return 语句
	return this->attributes;
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

uint16_t jdk::ClassFile::getSourceFileIndex() const
{
	return sourceFileIndex;
}

QString jdk::ClassFile::getSignature() const
{
	if (signatureIndex == -1) {
		return QString();
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignature(*attrInfo, *pool);
}

uint16_t jdk::ClassFile::getSignatureIndex() const
{
	if (signatureIndex == -1) {
		throw JdkException("not constaint signature", FILE_INFO);
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignatureConstantIndex(*attrInfo, *pool);
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


