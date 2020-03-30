#include "stdafx.h"
#include "field.h"

using namespace jdk;

uint16_t jdk::NameAndDescriptor::getAccessFlags()  const
{
	return accessFlags;
}

uint32_t jdk::NameAndDescriptor::getAccessFlagsOffset() const
{
	return accessFlagsOffset;
}

QString jdk::NameAndDescriptor::getName() const
{
	return cpool->getUtf8(nameIndex);
}

uint32_t jdk::NameAndDescriptor::getNameIndexOffset() const
{
	return nameIndexOffset;
}

QString jdk::NameAndDescriptor::getDescriptor() const
{
	return cpool->getUtf8(descriptorIndex);
}

uint32_t jdk::NameAndDescriptor::getDescriptorOffset() const
{
	return descriptorOffset;
}

uint32_t jdk::NameAndDescriptor::getLength() const
{
	return length;
}

uint16_t jdk::NameAndDescriptor::getSignatureIndex() const
{
	return signatureIndex;
}

QString jdk::NameAndDescriptor::getSignature() const
{
	if (signatureIndex == -1) {
		return QString();
	}
	auto attrInfo = attributes[signatureIndex];
	return SignatureReader::getSignature(*attrInfo, *cpool);
}

std::shared_ptr<NameAndDescriptor> jdk::NameAndDescriptor::create(const char* buffer, uint32_t offset, const ConstantPool* pool)
{
	std::shared_ptr<NameAndDescriptor> field(new NameAndDescriptor());
	field->cpool = pool;

	field->accessFlags = Util::readUnsignedInt16(buffer, offset);
	field->accessFlagsOffset = offset;

	field->nameIndex = Util::readUnsignedInt16(buffer, offset + 2);
	field->nameIndexOffset = offset + 2;

	field->descriptorOffset = offset + 4;
	field->descriptorIndex = Util::readUnsignedInt16(buffer, offset + 4);

	field->attributeCount = Util::readUnsignedInt16(buffer, offset + 6);

	uint32_t nextAttributeOffset = offset + 8;
	uint32_t attributeTotalLength = 0;
	for (int i = 0; i < field->attributeCount; i++) {
		auto attribute = AttributeInfo::createAttributeInfo(buffer, nextAttributeOffset, pool);
		field->attributes.push_back(attribute);
		nextAttributeOffset += attribute->getBodyLength() + 6;
		attributeTotalLength += attribute->getBodyLength() + 6;
		auto attrName = attribute->getAttributeName();
		if (QString::compare("Signature", attrName) == 0) {
			field->signatureIndex = i;
			QString sign = SignatureReader::getSignature(*attribute, *pool);
			qDebug() << "signature is:" << sign;
		}
		else if (QString::compare("Deprecated", attrName) == 0) {
			field->deprecatedIndex = i;
		}
	}

	field->length = attributeTotalLength + 8;

	return field;
}


jdk::NameAndDescriptor::NameAndDescriptor()
{
}
