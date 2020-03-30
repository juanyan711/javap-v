#include "stdafx.h"
#include "attributeInfo.h"

using namespace jdk;


jdk::AttributeInfo::AttributeInfo(uint16_t i, uint32_t len, const char* b, const uint32_t o, const ConstantPool* cp):
	attributeNameIndex(i), length(len), buffer(b), offset(o), cpool(cp)
{
}

QString jdk::AttributeInfo::getAttributeName() const
{
	return cpool->getUtf8(attributeNameIndex);
}

uint32_t jdk::AttributeInfo::getBodyLength() const
{
	return length;
}

uint32_t jdk::AttributeInfo::getOffset() const
{
	return offset;
}

const char* jdk::AttributeInfo::getBody() const
{
	return buffer + offset + 6;
}

void jdk::AttributeInfo::visit(IAttributeVisit& visitor)
{
	visitor.visit(this->shared_from_this());
}

std::shared_ptr<jdk::AttributeInfo> jdk::AttributeInfo::createAttributeInfo(const char* buffer, const uint32_t offset, const ConstantPool* cpool)
{
	uint16_t nameIndex = Util::readUnsignedInt16(buffer , offset);
	uint16_t length = Util::readUnsignedInt32(buffer, offset + 2);
	return std::shared_ptr<AttributeInfo>(new AttributeInfo(nameIndex, length, buffer, offset, cpool));
}

void jdk::AbstractVisit::visit(std::shared_ptr<AttributeInfo>& i)
{
	this->info = i;
	cpool = i->cpool;
	doVisit();
}

QString jdk::AbstractVisit::getAttributeName() const
{
	return info->getAttributeName();
}

uint32_t jdk::AbstractVisit::getBodyLength()
{
	return info->getBodyLength();
}
