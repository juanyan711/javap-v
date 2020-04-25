#pragma once
#include <vector>
#include "attributeInfo.h"
#include "Signature.h"
#include "code.h"

namespace jdk{
	//是Field和Method的公共体

	class NameAndDesc
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

		const std::vector<std::shared_ptr<AttributeInfo>>& getAttributesRef() const;

	
	protected:
		NameAndDesc();
		void initProperty(const char* buffer, uint32_t offset, const ConstantPool* pool);
		std::shared_ptr<AttributeInfo> newAttributeInfo(const char* buffer, uint32_t offset, uint32_t index, const ConstantPool* pool);
	protected:
		const ConstantPool* cpool;

		uint16_t accessFlags;
		uint32_t accessFlagsOffset;   //也就是首个字节的缓存区

		uint32_t nameIndexOffset;
		int16_t nameIndex;

		uint32_t descriptorOffset;
		int16_t descriptorIndex;

		uint16_t attributeCount;
		std::vector<std::shared_ptr<AttributeInfo>> attributes;

		int32_t length;   //包含了结构体中开始的8个字节的元数据
		int16_t signatureIndex = -1;
		int16_t deprecatedIndex = -1;

	};

	class MethodInfo :public NameAndDesc {
	public:

		std::shared_ptr<CodeAttribute> getCodeAttribute();

		std::shared_ptr<AttributeInfo> getParamters();

		static std::shared_ptr<MethodInfo> create(const char* buffer, uint32_t offset, const ConstantPool* pool);

	private:
		int32_t codeIndex = -1;
		int32_t parametersIndex = -1;

	};

	class FieldInfo :public NameAndDesc {
	public:
		static std::shared_ptr<FieldInfo> create(const char* buffer, uint32_t offset, const ConstantPool* pool);
	private:
		int16_t constantValueIndex = -1;
	};
}

