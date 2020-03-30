#pragma once
#include <vector>
#include "attributeInfo.h"
#include "Signature.h"

namespace jdk{
	//是Field和Method的公共体
	class NameAndDescriptor
	{
	public:
		uint16_t getAccessFlags() const;
		uint32_t getAccessFlagsOffset() const;

		QString getName() const;
		uint32_t getNameIndexOffset() const;

		QString getDescriptor() const;
		uint32_t getDescriptorOffset() const;

		uint32_t getLength() const;

		uint16_t getSignatureIndex() const;
		QString getSignature() const;

		static std::shared_ptr<NameAndDescriptor> create(const char* buffer, uint32_t offset, const ConstantPool* pool);
	
	private:
		NameAndDescriptor();
	private:
		const ConstantPool* cpool;

		uint16_t accessFlags;
		uint32_t accessFlagsOffset;   //也就是首个字节的缓存区

		uint32_t nameIndexOffset;
		uint16_t nameIndex;

		uint32_t descriptorOffset;
		uint16_t descriptorIndex;

		uint16_t fieldCount;
		std::vector<std::shared_ptr<NameAndDescriptor>> fields;

		uint16_t attributeCount;
		std::vector<std::shared_ptr<AttributeInfo>> attributes;

		uint32_t length;
		uint32_t signatureIndex = -1;
		uint32_t deprecatedIndex;

	};

	typedef NameAndDescriptor FieldInfo;
	typedef NameAndDescriptor MethodInfo;
}

