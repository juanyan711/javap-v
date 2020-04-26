#pragma once

#include"util.h"

#include<QString>
#include "attributeInfo.h"
#include<vector>
#include<tuple>

using std::vector;

namespace jdk {

	class SignatureVisitor;
	using SignatureVisitorPointer = std::shared_ptr<SignatureVisitor>;

	class SignatureVisitor :public std::enable_shared_from_this<SignatureVisitor> {
	public:

		static QChar EXTENDS;
		static QChar SUPER;
		static QChar INSTANCEOF;

		SignatureVisitor();

		virtual void visitFormalTypeParameter(QString& name);

		virtual SignatureVisitorPointer visitClassBound();

		virtual SignatureVisitorPointer visitInterfaceBound();

		virtual SignatureVisitorPointer visitSuperclass();

		virtual SignatureVisitorPointer visitInterface();

		virtual SignatureVisitorPointer visitParameterType();

		virtual SignatureVisitorPointer visitReturnType();

		virtual SignatureVisitorPointer visitExceptionType();

		virtual void visitBaseType(QChar& descriptor);

		virtual void visitTypeVariable(QString& name);

		virtual SignatureVisitorPointer visitArrayType();

		virtual void visitClassType(QString& name);

		virtual void visitInnerClassType(QString& name);

		virtual void visitTypeArgument();

		virtual SignatureVisitorPointer visitTypeArgument(QChar& wildcard);

		virtual void visitEnd();
	};

	class SignatureReader {
	public:
		SignatureReader(QString& v);
		void accept(SignatureVisitorPointer v);

		static QString getSignature(const AttributeInfo& attrInfo, const ConstantPool& cpool);
		static uint16_t getSignatureConstantIndex(const AttributeInfo& attrInfo, const ConstantPool& cpool);
	private:
		uint32_t parseType(QString& signature, int pos,
			SignatureVisitorPointer v);

	private:
		QString signatureValue;
	};

	class ClassSignatureVisitor :public SignatureVisitor {
	public:
		enum Step { start, formal, classz, super, interfaces };
		void visitFormalTypeParameter(QString& name) override;
		SignatureVisitorPointer visitClassBound() override;
		void visitClassType(QString& name) override;
		void visitInnerClassType(QString& name) override;
		void visitEnd() override;
		void visitBaseType(QChar&) override;
		SignatureVisitorPointer visitArrayType() override;
		QString toString();
		SignatureVisitorPointer visitSuperclass() override;
		SignatureVisitorPointer visitInterface() override;
		SignatureVisitorPointer visitInterfaceBound() override;
		void visitTypeVariable(QString& name) override;
		void visitTypeArgument() override;
		SignatureVisitorPointer visitTypeArgument(QChar&) override;
		

		void setFieldModel();

		ClassSignatureVisitor();
	private:
		Step s;
		QString buffer;
		int32_t open = 0;
		vector<int32_t> openStack;
		int32_t vdo = 0;
		vector<int32_t> vdoStack;
		bool fieldModel = false;

	};

	class MethodSignatureVisitor :public SignatureVisitor {
		enum Step { start, formal, paramter, returnParamter, throws };
	public:
		MethodSignatureVisitor();
		void visitFormalTypeParameter(QString& name) override;
		SignatureVisitorPointer visitClassBound() override;
		SignatureVisitorPointer visitInterfaceBound() override;
		SignatureVisitorPointer visitParameterType() override;
		void visitBaseType(QChar& descriptor) override;
		void visitTypeVariable(QString& name) override;
		SignatureVisitorPointer visitArrayType() override;
		void visitClassType(QString& name) override;
		void visitInnerClassType(QString& name) override;
		void visitEnd();

		void visitTypeArgument() override;
		SignatureVisitorPointer visitTypeArgument(QChar& wildcard) override;

		SignatureVisitorPointer visitReturnType() override;
		SignatureVisitorPointer visitExceptionType() override;

		QString getParamSignature();
		QString getRetureSignature();
		QString getThrowsSignature();
		QString getTypeParameters();

	private:
		Step s;
		int32_t paramOffset = 0;
		int32_t returnOffset = 0;
		int32_t throwsOffset = -1;
		QString buffer;
		int32_t open = 0;
		vector<int32_t> openStack;
		int32_t vdo = 0;
		vector<int32_t> vdoStack;
	};


}


