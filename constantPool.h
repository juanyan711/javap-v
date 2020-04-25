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

	class ConstantTag {
	public:
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

		static QString translate(uint8_t t, bool constant = true);
	};

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
		void setConstantPool(const ConstantPool* src);
		virtual QString toString() const = 0;
	protected:
		uint8_t tag;
		uint32_t offset;    //跳过了前面的一个类型字段
		uint32_t length;   //不包含tag字段占据的空间
		const ConstantPool* pool;
	};

	class ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset) = 0;
	};

	class Utf8Info :public CpInfo {
	public:
		Utf8Info(const char* b, uint16_t len, uint32_t o);

		uint32_t getByteLength();   //标准定义的是uint16_t
		QString toString() const override;
		QByteArray getByteArray();
	protected:
		const char* bytes;   //外部缓存中utf8数组的首字节的地址
		uint16_t utf8ByteLength;
	};

	class Utf8InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	class SimpleRefInfo :public CpInfo {
	public:
		SimpleRefInfo(uint16_t i, uint32_t off, uint16_t tag);
		uint16_t getIndex() const;
		QString toString() const override;
	private:
		uint16_t index;
	};

	class ClassInfo :public SimpleRefInfo {
	public:
		static const QString OBJECT;
	public:
		ClassInfo(uint16_t, uint32_t off);
		QString toString() const override;
	};

	class StringInfo :public SimpleRefInfo {
	public:
		StringInfo(uint16_t, uint32_t);
		QString toString() const override;
	};

	class MethodTypeInfo :public SimpleRefInfo {
	public:
		MethodTypeInfo(uint16_t d, uint32_t o);
	};

	class U2InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset) override;
	};

	class BaseRefInfo :public CpInfo {
	public:
		BaseRefInfo(uint16_t cls, uint16_t nameAndType, uint32_t off, uint16_t tag);
		uint16_t getIndex1() const;
		uint16_t getIndex2() const;

		QString toString() const override;
	protected:
		uint16_t index1;   //表示这个属性属于什么类, classIndex
		uint16_t index2;    //具体的名称和索引
	};

	class InsiderRefInfo :public BaseRefInfo {
	public:
		InsiderRefInfo(uint16_t cls, uint16_t nameAndType, uint32_t off, uint16_t tag);
		uint16_t getClassIndex();
		uint16_t getNameAndTypeIndex();
	};

	class FieldRefInfo :public InsiderRefInfo {
	public:
		FieldRefInfo(uint16_t ci, uint16_t ni, uint32_t off);
	};

	class MethodRefInfo :public InsiderRefInfo {
	public:
		MethodRefInfo(uint16_t ci, uint16_t ni, uint32_t off);
	};

	class InterfaceRefInfo :public InsiderRefInfo {
	public:
		InterfaceRefInfo(uint16_t ci, uint16_t ni, uint32_t off);
	};

	class NameAndTypeInfo :public BaseRefInfo {
	public:
		NameAndTypeInfo(uint16_t, uint16_t, uint32_t);

		uint16_t getNameIndex() const;
		uint16_t getDescriptIndex() const;

		QString toString() const override;

	};

	class InvokeDynamicInfo :public BaseRefInfo {
	public:
		InvokeDynamicInfo(uint16_t, uint16_t, uint32_t);

		uint16_t getBootstrapMethodAttrIndex() const;
		uint16_t getNameAndTypeIndex() const;

		QString toString() const override;

	
	};

	class RefAndDynamicInfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char*, uint32_t offset);
	};

	template<typename T, uint16_t TAG>
	class U4Info :public CpInfo {
	public:
		U4Info(T v, uint32_t o) :CpInfo(TAG, o), value(v) {
			length = 4;
		}
		QString toString() const override {
			if (tag == ConstantTag::CONSTANT_FLOAT) {
				int64_t integer = floor(value);
				int32_t pre = 7 + Util::digits(integer);
				QString num = QString::number(value, 'g', pre).append("f");
				return num;
			}
			else {
				return QString::number(value);
			}
		}
	protected:
		T value;
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
		U8Info(uint32_t high, uint32_t low, uint32_t o, T v) :CpInfo(TAG, o), high_bytes(high), low_bytes(low), value(v){
			length = 8;
		}

		QString toString() const {
			if (tag == ConstantTag::CONSTANT_DOUBLE) {
				int64_t integer = floor(value);
				int32_t pre = 14 + Util::digits(integer);
				return QString::number(value, 'g', pre).append("d");
			}
			return QString::number(value).append("l");
		}

	protected:
		uint32_t high_bytes;
		uint32_t low_bytes;
		T value;
	};

	using LongInfo = U8Info<int64_t, ConstantTag::CONSTANT_LONG>;
	using DoubleInfo = U8Info<double, ConstantTag::CONSTANT_DOUBLE>;

	class U8InfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};

	class MethodHandleInfo :public CpInfo {
	public:
		MethodHandleInfo(uint8_t rk, uint16_t ri, uint32_t o);

		uint16_t getReferenceKind() const;
		uint16_t getReferenceIndex() const;

		QString toString() const override;
		
	protected:
		uint8_t kind;
		uint16_t referenceIndex;
	};

	class MethodHandleInfoFactory :public ICpInfoFactory {
	public:
		virtual std::unique_ptr<CpInfo> createCpInfo(uint8_t tag, const char* b, uint32_t offset);
	};


	class ConstantPool{
	private:
		ConstantPool(uint16_t, uint32_t);
	public:
		void add(const std::shared_ptr<CpInfo>&);
		std::shared_ptr<CpInfo> at(uint32_t index) const;
		uint32_t getLength() const;
		uint16_t getConstantItemNumber() const;

		QString getClassName(uint16_t classIndex) const;  //根据classIndex来查询class的名称
		QString getUtf8(uint16_t utf8Index)  const;   //查询字符串
		QString getNameAndType(uint16_t ntIndex) const;  
		QString getMethodRef(uint16_t mrIndex) const;
		QString getFieldRef(uint16_t mrIndex) const;
		QString getGeneral(uint16_t index) const;

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
		uint32_t length;    //常量池长度,包含了开始的两个constantPool-length字节
		uint32_t offset;
		static std::vector<std::shared_ptr<ICpInfoFactory>> cpInfoFactorys;
	};



}



