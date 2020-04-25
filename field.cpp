#include "stdafx.h"
#include "field.h"

using namespace jdk;

uint16_t jdk::NameAndDesc::getAccessFlags()  const
{
	return accessFlags;
}

uint32_t jdk::NameAndDesc::getAccessFlagsOffset() const
{
	return accessFlagsOffset;
}

QString jdk::NameAndDesc::getName() const
{
	return cpool->getUtf8(nameIndex);
}

uint32_t jdk::NameAndDesc::getNameIndexOffset() const
{
	return nameIndexOffset;
}

QString jdk::NameAndDesc::getDescriptor() const
{
	return cpool->getUtf8(descriptorIndex);
}

uint32_t jdk::NameAndDesc::getDescriptorOffset() const
{
	return descriptorOffset;
}

uint32_t jdk::NameAndDesc::getLength() const
{
	return length;
}

uint16_t jdk::NameAndDesc::getSignatureIndex() const
{
	if (signatureIndex == -1) {
		throw JdkException("no signature", FILE_INFO);
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignatureConstantIndex(*attrInfo, *cpool);
}

QString jdk::NameAndDesc::getSignature() const
{
	if (signatureIndex == -1) {
		return QString();
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignature(*attrInfo, *cpool);
}

std::shared_ptr<CodeAttribute> jdk::MethodInfo::getCodeAttribute()
{
	if (codeIndex == -1) {
		if ((this->getAccessFlags() & AccessFlags::ACC_ABSTRACT) == 0) {
			throw JdkException("no code attribute", FILE_INFO);
		}
		else {
			return shared_ptr<CodeAttribute>(nullptr);
		}
	}
	auto attr = attributes[codeIndex];
	return std::shared_ptr<CodeAttribute>(new CodeAttribute(attributes[codeIndex].get(), cpool));
}

std::shared_ptr<AttributeInfo> jdk::MethodInfo::getParamters()
{
	// TODO: 在此处插入 return 语句
	if (parametersIndex == -1) {
		return std::shared_ptr<AttributeInfo>(nullptr);
	}
	return attributes[parametersIndex];
}

const std::vector<std::shared_ptr<AttributeInfo>>& jdk::NameAndDesc::getAttributesRef() const
{
	// TODO: 在此处插入 return 语句
	return attributes;
}

std::shared_ptr<MethodInfo> jdk::MethodInfo::create(const char* buffer, uint32_t offset, const ConstantPool* pool)
{
	std::shared_ptr<MethodInfo> method(new MethodInfo());
	method->initProperty(buffer, offset, pool);

	uint32_t nextAttributeOffset = offset + 8;
	uint32_t attributeTotalLength = 0;
	for (int i = 0; i < method->attributeCount; i++) {
		auto attribute = method->newAttributeInfo(buffer, nextAttributeOffset, i, pool);
		auto attrName = attribute->getAttributeName();
		if (QString::compare("Code", attrName) == 0) {
			method->codeIndex = i;
		}
		else if (QString::compare("MethodParameters", attrName) == 0) {
			method->parametersIndex = i;
		}
		nextAttributeOffset += attribute->getBodyLength() + 6;
		attributeTotalLength += attribute->getBodyLength() + 6;
	}

	method->length = attributeTotalLength + 8;

	return method;
}


jdk::NameAndDesc::NameAndDesc()
{
}

void jdk::NameAndDesc::initProperty(const char* buffer, uint32_t offset, const ConstantPool* pool)
{
	this->cpool = pool;

	this->accessFlags = Util::readUnsignedInt16(buffer, offset);
	this->accessFlagsOffset = offset;

	this->nameIndex = Util::readUnsignedInt16(buffer, offset + 2);
	this->nameIndexOffset = offset + 2;

	this->descriptorOffset = offset + 4;
	this->descriptorIndex = Util::readUnsignedInt16(buffer, offset + 4);

	this->attributeCount = Util::readUnsignedInt16(buffer, offset + 6);
}

std::shared_ptr<AttributeInfo> jdk::NameAndDesc::newAttributeInfo(const char* buffer, uint32_t nextAttributeOffset, uint32_t index, const ConstantPool* pool)
{
	auto attribute = AttributeInfo::createAttributeInfo(buffer, nextAttributeOffset, pool);
	this->attributes.push_back(attribute);

	auto attrName = attribute->getAttributeName();
	qDebug() << "attribute name: " << attrName;
	if (QString::compare("Signature", attrName) == 0) {
		this->signatureIndex = index;
		QString sign = SignatureReader::getSignature(*attribute, *pool);
		qDebug() << "signature is:" << sign;
	}
	else if (QString::compare("Deprecated", attrName) == 0) {
		this->deprecatedIndex = index;
	}
	return attribute;
}

std::shared_ptr<FieldInfo> jdk::FieldInfo::create(const char* buffer, uint32_t offset, const ConstantPool* pool)
{
	std::shared_ptr<FieldInfo> field(new FieldInfo());
	field->initProperty(buffer, offset, pool);

	uint32_t nextAttributeOffset = offset + 8;
	uint32_t attributeTotalLength = 0;
	for (int i = 0; i < field->attributeCount; i++) {
		auto attribute = field->newAttributeInfo(buffer, nextAttributeOffset, i, pool);
		auto attrName = attribute->getAttributeName();
		if (QString::compare("ConstantValue", attrName) == 0) {
			field->constantValueIndex = i;
		}
		nextAttributeOffset += attribute->getBodyLength() + 6;
		attributeTotalLength += attribute->getBodyLength() + 6;
	}

	field->length = attributeTotalLength + 8;

	return field;
}
