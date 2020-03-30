#pragma once

#include<stdint.h>
#include<QByteArray>
#include<memory>
#include<vector>
#include"util.h"
#include<QDebug>

#include "exceptions.h"

namespace jdk {

	class ConstantPool;

	typedef struct  {
		static const uint8_t CONSTANT_CLASS = 7;
		static const uint8_t CONSTANT_FIELD_REF = 9;
		static const uint8_t CONSTANT_METHOD_REF = 10;
		static const uint8_t CONSTANT_INTERFACE_METHOD_REF = 11;
		static const uint8_t CONSTANT_STRING = 8;
		static const uint8_t CONSTANT_INTEGER = 3;
		static const uint8_t CONSTANT_FLOAT = 4;
		static const uint8_t CONSTANT_LONG = 5;
		static const uint8_t CONSTANT_DOUBLE = 6;
		static const uint8_t CONSTANT_NAME_AND_TYPE = 12;
		static const uint8_t CONSTANT_UTF8 = 1;
		static const uint8_t CONSTANT_METHOD_HANDLE = 15;
		static const uint8_t CONSTANT_METHOD_TYPE = 16;
		static const uint8_t CONSTANT_INVOKE_DYNAMIC = 18;
	} ConstantTag;

	class CpInfo
	{
	public:
		CpInfo(uint8_t);
		CpInfo(uint8_t t, uint32_t off);
		virtual ~CpInfo();
		uint8_t getTag();
		void setOffset(uint32_t off);
		uint32_t getOffset();
		uint32_t getLength();
		void setConstantPool(std::shared_ptr<ConstantPool>& src);
	protected:
		uint8_t tag;
		uint32_t offset;    //跳过了前面的一个类型字段
		uint32_t length;   //不包含tag字段占据的空间
		std::weak_ptr<ConstantPool> pool;
	};

	class ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset) = 0;
	};

	class ClassInfo :public CpInfo {
	public:
		static const QString OBJECT;
	public:
		ClassInfo(uint16_t nameIndex, uint32_t off);
		QString getClassName();
		uint16_t getNameIndex() const;
	protected:
		uint16_t nameIndex;
	};

	class U2InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset) override;
	};

	template<uint16_t TAG>
	class RefInfo :public CpInfo {
	public:
		RefInfo(uint16_t cls, uint16_t nameAndType, uint32_t off) :CpInfo(TAG, off), classIndex(cls), nameAndTypeIndex(nameAndType) {
			length = 4;
		}
		uint16_t getClassIndex() {
			return classIndex;
		}
		uint16_t getNameAndTypeIndex() {
			return nameAndTypeIndex;
		}
	protected:
		uint16_t classIndex;   //表示这个属性属于什么类
		uint16_t nameAndTypeIndex;    //具体的名称和索引
	};

	using FieldRefInfo = RefInfo < ConstantTag::CONSTANT_FIELD_REF>;
	using MethodRefInfo =  RefInfo<ConstantTag::CONSTANT_METHOD_REF>;
	using InterfaceRefInfo = RefInfo<ConstantTag::CONSTANT_INTERFACE_METHOD_REF>;

	class RefAndDynamicInfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset);
	};

	class StringInfo :public CpInfo {
	public:
		StringInfo(uint16_t, uint32_t);
		uint16_t getUtf8Index();
		QString getString();
	protected:
		uint16_t utf8Index;
	};

	template<typename T, uint16_t TAG>
	class U4Info :public CpInfo {
	public:
		U4Info(T v, uint32_t o) :CpInfo(TAG, o), bytes(v) {
			length = 4;
		}
	protected:
		T bytes;
	};

	using IntegerInfo = U4Info<uint32_t, ConstantTag::CONSTANT_INTEGER>;
	using FloatInfo = U4Info<float, ConstantTag::CONSTANT_FLOAT>;

	class U4InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset);
	};

	template<typename T, uint16_t TAG>
	class U8Info :public CpInfo {
	public:
		U8Info(uint32_t high, uint32_t low, uint32_t o) :CpInfo(TAG, o), high_bytes(high), low_bytes(low) {
			length = 8;
		}

	protected:
		uint32_t high_bytes;
		uint32_t low_bytes;
		T value;
	};

	using LongInfo = U8Info<uint64_t, ConstantTag::CONSTANT_LONG>;
	using DoubleInfo = U8Info<uint64_t, ConstantTag::CONSTANT_DOUBLE>;

	class U8InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	class NameAndTypeInfo :public CpInfo {
	public:
		NameAndTypeInfo(uint16_t, uint16_t, uint32_t );

		uint16_t getNameIndex();
		uint16_t getDescriptIndex();

	protected:
		uint16_t nameIndex;
		uint16_t descriptIndex;
	};

	class NameAndTypeInfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	class Utf8Info :public CpInfo {
	public:
		Utf8Info(const char* b, uint16_t len, uint32_t o);

		uint32_t getByteLength();   //标准定义的是uint16_t
		QString getString();
		QByteArray getByteArray();
	protected:
		const char* bytes;   //外部缓存中utf8数组的首字节的地址
		uint16_t utf8ByteLength;
	};

	class Utf8InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	enum class ReferenceKind {getField = 1, getStatic, putField, putStatic, invokeVirtual, invokeStatic, invokeSpecial, newInvokeSpecial, invokeInterface};

	class MethodHandleInfo :public CpInfo {
	public:
		MethodHandleInfo(ReferenceKind rk, uint16_t ri, uint32_t o);
		
	protected:
		ReferenceKind kind;
		uint16_t referenceIndex;
	};

	class MethodHandleInfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	class MethodTypeInfo :public CpInfo {
	public:
		MethodTypeInfo(uint16_t d, uint32_t o);
	protected:
		uint16_t descriptorIndex;
	};

	class InvokeDynamicInfo :public CpInfo {
	public:
		InvokeDynamicInfo(uint16_t, uint16_t, uint32_t);
	protected:
		uint16_t bootstrapMethodAttrIndex;
		uint16_t nameAndTypeIndex;
	};

	class ConstantPool{
	private:
		ConstantPool(uint16_t, uint32_t);
	public:
		void add(const std::shared_ptr<CpInfo>&);
		std::shared_ptr<CpInfo> at(uint32_t index) const;
		uint32_t getLength() const;

		QString getClassName(uint16_t classIndex) const;  //根据classIndex来查询class的名称
		QString getUtf8(uint16_t utf8Index)  const;   //查询字符串
		QString getNameAndType(uint16_t ntIndex) const;  
		QString getMethodRef(uint16_t mrIndex) const;

		template<typename F>
		void each(F func) {
			for (uint32_t i = 0; i < pool.size(); i++) {
				auto cpInfo_ptr = pool[i];
				if (cpInfo_ptr.get() == nullptr) {
					continue;
				}
				func(cpInfo_ptr, i);
			}
		}

		static std::shared_ptr<ConstantPool> createConstantPool(QByteArray& bytes, uint32_t offset);
	private:
		std::vector<std::shared_ptr<CpInfo>> pool;
		uint16_t cpInfoSize;   //常量池数组容量,index=0的值是一个空值, Long和Double占据两个索引值
		uint32_t length;    //常量池长度
		uint32_t offset;
		static std::vector<std::shared_ptr<ICpInfoFactory>> cpInfoFactorys;
	};



}



