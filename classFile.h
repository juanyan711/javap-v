#pragma once

#include<QByteArray>
#include"constantPool.h"
#include "attributeInfo.h"
#include "field.h"

#include<set>
#include<array>


namespace jdk {
	class  ClsVersion {
	public:
		static const uint32_t V1_1 = 3 << 16 | 45;
		static const uint32_t V1_2 = 0 << 16 | 46;
		static const uint32_t V1_3 = 0 << 16 | 47;
		static const uint32_t V1_4 = 0 << 16 | 48;
		static const uint32_t V1_5 = 0 << 16 | 49;
		static const uint32_t V1_6 = 0 << 16 | 50;
		static const uint32_t V1_7 = 0 << 16 | 51;
		static const uint32_t V1_8 = 0 << 16 | 52;
		static const uint32_t V9 = 0 << 16 | 53;
		static const uint32_t V10 = 0 << 16 | 54;
		static const uint32_t V11 = 0 << 16 | 55;
		static const uint32_t V12 = 0 << 16 | 56;

		static bool check(uint32_t ver);
	} ;

	enum {
		// As specifed in the JVM spec
		ITEM_Top = 0,
		ITEM_Integer = 1,
		ITEM_Float = 2,
		ITEM_Double = 3,
		ITEM_Long = 4,
		ITEM_Null = 5,
		ITEM_UninitializedThis = 6,
		ITEM_Object = 7,
		ITEM_Uninitialized = 8,
		ITEM_Bogus = (uint)-1
	};

	

	class ClassFile
	{
	public:
		ClassFile(const QString& fn, const QByteArray& src, const uint32_t offset = 0);

		void load(bool check);
		uint32_t getOffset() const;
		QString getFileName() const;
		const char* getBuffer() const;
		uint32_t getBufferSize() const;

		uint32_t getMagic() const;
		QString getMagicValue() const;
		uint16_t getMinor() const;
		uint16_t getMajor() const;

		uint16_t getAccessFlags() const;
		uint32_t getAccessFlagsOffset() const;

		QString getThisClass() const;
		uint32_t getThisClassOffset() const;
		uint16_t getThisClassIndex() const;

		QString getSuperClass() const;
		uint32_t getSuperClassOffset() const;
		uint16_t getSuperClassIndex() const;

		uint32_t getInterfaceOffset() const;
		const std::vector<QString> getInterfaces() const;   //不能返回引用，因为vector是一个局部变量
		uint16_t getInterfaceCount() const;

		uint32_t getFieldCount() const;
		const std::vector<std::shared_ptr<FieldInfo>>& getFields() const;

		uint32_t getMethodCount() const;
		const std::vector<std::shared_ptr<MethodInfo>>& getMethods() const;

		uint32_t getAttributeCount() const;
		const std::vector<std::shared_ptr<AttributeInfo>>& getAttributes() const;

		std::shared_ptr<ConstantPool> getConstantPool() const;

		QString getSourceFile() const;
		uint16_t getSourceFileIndex() const;
		QString getSignature() const;
		uint16_t getSignatureIndex() const;


	private:
		QString fileName;
		QByteArray src;
		uint32_t offset;

		uint32_t magic;
		uint16_t minor_version;
		uint16_t major_version;

		std::shared_ptr<ConstantPool>  pool;
		
		uint16_t accessFlags;
		uint32_t accessFlagsOffset;

		uint16_t thisClassIndex;
		uint32_t thisClassOffset;

		uint16_t superClassIndex;
		uint32_t superClassOffset;

		uint32_t interfaceOffset;
		uint16_t interfaceCount;
		std::vector<uint16_t> interfaceIndexs;

		uint16_t fieldCount;
		std::vector<std::shared_ptr<FieldInfo>> fields;

		uint16_t methodCount;
		std::vector<std::shared_ptr<MethodInfo>> methods;

		uint16_t attributeCount;
		std::vector<std::shared_ptr<AttributeInfo>> attributes;

		int32_t sourceFileIndex = -1;
		int32_t signatureIndex = -1;
		int32_t deprecatedIndex = -1;
	};

}


