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

	class AccessFlags {
	public:
		static const uint16_t ACC_PUBLIC = 0x0001;         // class, inner, field, method
		static const uint16_t ACC_PRIVATE = 0x0002;        //        inner, field, method
		static const uint16_t ACC_PROTECTED = 0x0004;      //        inner, field, method
		static const uint16_t ACC_STATIC = 0x0008;         //        inner, field, method
		static const uint16_t ACC_FINAL = 0x0010;          // class, inner, field, method
		static const uint16_t ACC_SUPER = 0x0020;          // class
		static const uint16_t ACC_SYNCHRONIZED = 0x0020;   //                      method
		static const uint16_t ACC_VOLATILE = 0x0040;       //               field
		static const uint16_t ACC_BRIDGE = 0x0040;         //                      method
		static const uint16_t ACC_TRANSIENT = 0x0080;      //               field
		static const uint16_t ACC_VARARGS = 0x0080;        //                      method
		static const uint16_t ACC_NATIVE = 0x0100;         //                      method
		static const uint16_t ACC_INTERFACE = 0x0200;      // class, inner
		static const uint16_t ACC_ABSTRACT = 0x0400;       // class, inner,        method
		static const uint16_t ACC_STRICT = 0x0800;         //                      method
		static const uint16_t ACC_SYNTHETIC = 0x1000;      // class, inner, field, method
		static const uint16_t ACC_ANNOTATION = 0x2000;     // class, inner
		static const uint16_t ACC_ENUM = 0x4000;           // class, inner, field
		static const uint16_t ACC_MODULE = 0x8000;         // class, inner, field, method
		enum Kind { Class, InnerClass, Field, Method };

	private:
		static const std::array<uint16_t,4> classModifiers;
		static const std::array<uint16_t,9> classFlags;
		static const std::array<uint16_t,7> innerClassModifiers;
		static const std::array<uint16_t,12> innerClassFlags;
		static const std::array<uint16_t,8> fieldModifiers;
		static const std::array<uint16_t,10> fieldFlags;
		static const std::array<uint16_t,10> methodModifiers;
		static const std::array<uint16_t,13> methodFlags;

		static QString flagToModifier(uint16_t flag, Kind t);
		static QString flagToName(uint16_t flag, Kind t);

	public:
		std::set<QString> getClassModifiers();

		std::set<QString> getClassFlags();

		std::set<QString> getInnerClassModifiers();

		std::set<QString> getInnerClassFlags();

		std::set<QString> getFieldModifiers();

		std::set<QString> getFieldFlags();

		std::set<QString> getMethodModifiers();

		std::set<QString> getMethodFlags();

		template<uint32_t N>
		std::set<QString> getModifiers(std::array<uint16_t, N> modifierFlags, Kind t) {
			return getModifiers<N>(flags, modifierFlags, t);
		}

	private:
		template<uint32_t N>
		static std::set<QString> getModifiers(uint16_t flags, std::array<uint16_t, N> modifierFlags, Kind t) {
			std::set<QString> s;
			for (auto m : modifierFlags) {
				if ((flags & m) != 0)
					s.emplace(flagToModifier(m, t));
			}
			return std::move(s);
		}

		template<uint32_t N>
		std::set<QString> getFlags(std::array<uint16_t, N> expectedFlags, Kind t) {
			std::set<QString> s;
			int f = flags;
			for (auto e : expectedFlags) {
				if ((f & e) != 0) {
					s.emplace(flagToName(e, t));
					f = f & ~e;
				}
			}
		/*	while (f != 0) {
				int bit = Integer.highestOneBit(f);
				s.emplace("0x" + Integer.toHexString(bit));
				f = f & ~bit;
			}*/
			return std::move(s);
		}
	
	public:
		AccessFlags(uint16_t fs);
		AccessFlags ignore(int mask);
		bool is(int mask);
	private:
		const uint16_t flags;
	};

	class ClassFile
	{
	public:
		ClassFile(const QString& fn, const QByteArray& src, const uint32_t offset = 0);
		QString getFileName() const;
		const char* getBuffer() const;

		uint32_t getMagic() const;
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

		std::shared_ptr<ConstantPool> getConstantPool() const;

		QString getSourceFile() const;
		QString getSignature() const;


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


