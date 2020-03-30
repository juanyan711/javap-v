#pragma once

#include "constantpool.h"

namespace jdk {
	/*
	attribute_info{
	 u2 attribute_name_index;
	 u4 attribute_length;
	 u1 attribute[attribute_length];
	}
	*/
	class IAttributeVisit;
	class AttributeInfo:public std::enable_shared_from_this<AttributeInfo>
	{
		friend class AbstractVisit;
	private:
		AttributeInfo(uint16_t i, uint32_t len, const char* b, const uint32_t o, const ConstantPool* cp);
	public:
		QString getAttributeName() const;
		uint32_t getBodyLength() const;
		uint32_t getOffset() const;
		const char* getBody() const;
		void visit(IAttributeVisit&);
		static std::shared_ptr<AttributeInfo> createAttributeInfo(const char* buffer, const uint32_t offset, const ConstantPool* cpool);

		template<typename T>
		void visit(T ft) const;

	protected:
		uint16_t attributeNameIndex;    //属性名索引，解析后的值
		uint32_t length;       // 长度，解析后的值,body_length
		const char* buffer;    //文件的缓存
		const uint32_t offset;   //nameIndex的偏移量
		const ConstantPool* cpool;
	};

	class IAttributeVisit {
	public:
		//不能作为s
		virtual void visit(std::shared_ptr<AttributeInfo>& info) = 0;
	};

	class AbstractVisit:public IAttributeVisit {
	public:
		void visit(std::shared_ptr<AttributeInfo>& info) override;
		virtual void doVisit() = 0;
		QString getAttributeName() const;
		uint32_t getBodyLength();
	private:
		std::shared_ptr<AttributeInfo> info;
		const ConstantPool* cpool;
	};

	template<typename T>
	inline void AttributeInfo::visit(T ft) const
	{
		ft(this->shared_from_this());
	}

}


