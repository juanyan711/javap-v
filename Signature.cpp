#include "stdafx.h"
#include "Signature.h"


using namespace jdk;

QString jdk::SignatureReader::getSignature(const AttributeInfo& attrInfo, const ConstantPool& cpool)
{
	uint16_t utf8Index = 0;
	attrInfo.visit([&utf8Index](std::shared_ptr<const AttributeInfo> info) {
		const char* body = info->getBody();
		utf8Index = Util::readUnsignedInt16(body, 0);
		});
	return cpool.getUtf8(utf8Index);
}

uint16_t jdk::SignatureReader::getSignatureConstantIndex(const AttributeInfo& attrInfo, const ConstantPool& cpool)
{
	uint16_t utf8Index = 0;
	attrInfo.visit([&utf8Index](std::shared_ptr<const AttributeInfo> info) {
		const char* body = info->getBody();
		utf8Index = Util::readUnsignedInt16(body, 0);
		});
	return utf8Index;
}

jdk::SignatureReader::SignatureReader(QString& v):signatureValue(v)
{
}

void jdk::SignatureReader::accept(SignatureVisitorPointer v)
{
	QString& signature = this->signatureValue;
	uint32_t len = signature.size();
	uint32_t pos;
	QChar c;

	if (signature.at(0) == QChar('<')) {
		pos = 2;
		do {
			uint32_t end = signature.indexOf(QChar(':'), pos);
			v->visitFormalTypeParameter(signature.mid(pos - 1, end - pos + 1));
			pos = end + 1;

			c = signature.at(pos);
			if (c == QChar('L') || c == QChar('[') || c == QChar('T')) {
				pos = parseType(signature, pos, v->visitClassBound());
			}

			while ((c = signature.at(pos++)) == ':') {
				pos = parseType(signature, pos, v->visitInterfaceBound());
			}
		} while (c != QChar('>'));
	}
	else {
		pos = 0;
	}

	if (signature.at(pos) == '(') {
		pos++;
		while (signature.at(pos) != ')') {
			pos = parseType(signature, pos, v->visitParameterType());
		}
		//pos + 1, skip ')'
		pos = parseType(signature, pos + 1, v->visitReturnType());
		while (pos < len) {
			//pos + 1 , skip ^
			pos = parseType(signature, pos + 1, v->visitExceptionType());
		}
	}
	else {
		pos = parseType(signature, pos, v->visitSuperclass());
		while (pos < len) {
			pos = parseType(signature, pos, v->visitInterface());
		}
	}
}

uint32_t jdk::SignatureReader::parseType(QString& signature, int pos, SignatureVisitorPointer v)
{
	uint32_t start, end;
	boolean visited, inner;
	QString name;
	QChar c = signature.at(pos++);

	switch (c.toLatin1()) {
	case 'Z':
	case 'C':
	case 'B':
	case 'S':
	case 'I':
	case 'F':
	case 'J':
	case 'D':
	case 'V':
		v->visitBaseType(c);
		return pos;

	case '[':
		return parseType(signature, pos, v->visitArrayType());

	case 'T':
		end = signature.indexOf(';', pos);
		v->visitTypeVariable(signature.mid(pos, end - pos));
		return end + 1;

	default: // case 'L':
		start = pos;
		visited = false;
		inner = false;
		for (;;) {
			c = signature.at(pos++);
			switch (c.toLatin1()) {
			case '.':
			case ';':
				if (!visited) {
					name = signature.mid(start, pos - 1 - start);
					if (inner) {
						v->visitInnerClassType(name);
					}
					else {
						v->visitClassType(name);
					}
				}
				if (c == ';') {
					v->visitEnd();
					return pos;
				}
				start = pos;
				visited = false;
				inner = true;
				break;

			case '<':
				name = signature.mid(start, pos - 1 - start);
				if (inner) {
					v->visitInnerClassType(name);
				}
				else {
					v->visitClassType(name);
				}
				visited = true;
				while ((c = signature.at(pos))!=QChar('>')) {
					switch (c.toLatin1()) {
					case '*':
						++pos;
						v->visitTypeArgument();
						break;
					case '+':
					case '-':
						pos = parseType(signature, pos + 1, v->visitTypeArgument(c));
						break;
					default:
						pos = parseType(signature, pos,
							v->visitTypeArgument(QChar('=')));
						break;
					}
				}
			}
		}
	}
}

QChar jdk::SignatureVisitor::EXTENDS = '+';
QChar jdk::SignatureVisitor::SUPER = '-';
QChar jdk::SignatureVisitor::INSTANCEOF = '=';

jdk::SignatureVisitor::SignatureVisitor()
{
}

void jdk::SignatureVisitor::visitFormalTypeParameter(QString& name)
{
}

SignatureVisitorPointer jdk::SignatureVisitor::visitClassBound()
{
	return this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitInterfaceBound()
{
	return  this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitSuperclass()
{
	return  this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitInterface()
{
	return  this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitParameterType()
{
	return  this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitReturnType()
{
	return  this->shared_from_this();
}

SignatureVisitorPointer jdk::SignatureVisitor::visitExceptionType()
{
	return  this->shared_from_this();
}

void jdk::SignatureVisitor::visitBaseType(QChar& descriptor)
{
}

void jdk::SignatureVisitor::visitTypeVariable(QString& name)
{
}

SignatureVisitorPointer jdk::SignatureVisitor::visitArrayType()
{
	return  this->shared_from_this();
}

void jdk::SignatureVisitor::visitClassType(QString& name)
{
}

void jdk::SignatureVisitor::visitInnerClassType(QString& name)
{
}

void jdk::SignatureVisitor::visitTypeArgument()
{
}

SignatureVisitorPointer jdk::SignatureVisitor::visitTypeArgument(QChar& wildcard)
{
	return  this->shared_from_this();
}

void jdk::SignatureVisitor::visitEnd()
{
}

void jdk::ClassSignatureVisitor::visitFormalTypeParameter(QString& name)
{
	if (s == Step::start) {
		s = Step::formal;
		buffer.append("<");
	}
	else if (s == Step::formal) {
		buffer.append(", ");
	}
	buffer.append(name);
}

SignatureVisitorPointer jdk::ClassSignatureVisitor::visitClassBound()
{
	buffer.append(" extends ");
	return SignatureVisitor::visitClassBound();
}

void jdk::ClassSignatureVisitor::visitClassType(QString& name)
{
	auto name2 = name.replace("/", ".");
	buffer.append(name2);
	lastIsClass = true;
}

void jdk::ClassSignatureVisitor::visitInnerClassType(QString& name)
{
	//auto name2 = name.replace("/", ".");
	if (open > 0) {
		buffer.append(">");
		open--;
	}
	buffer.append(".");
	buffer.append(name);
	lastIsClass = true;
}

void jdk::ClassSignatureVisitor::visitBaseType(QChar& b)
{
	buffer.append(Util::baseTypeName(b));
}

QString jdk::ClassSignatureVisitor::toString()
{
	return buffer;
}

SignatureVisitorPointer jdk::ClassSignatureVisitor::visitSuperclass()
{
	if (s == Step::formal) {
		buffer.append(">");
		s = Step::super;
	}
	else if (s == Step::start) {
		s = Step::super;
	}
	if (!fieldModel) {
		buffer.append(" extends ");
	}
	
	return SignatureVisitor::visitSuperclass();
}

SignatureVisitorPointer jdk::ClassSignatureVisitor::visitInterface()
{
	if (s == Step::interfaces) {
		buffer.append(", ");
	}
	else {
		s = Step::interfaces;
		buffer.append(" implements ");
	}
	
	return SignatureVisitor::visitInterface();
}

SignatureVisitorPointer jdk::ClassSignatureVisitor::visitInterfaceBound()
{
	if (s == Step::formal) {
		buffer.append(" & ");
	}
	return SignatureVisitor::visitInterfaceBound();
}

void jdk::ClassSignatureVisitor::visitTypeVariable(QString& name)
{
	buffer.append(name);
	lastIsClass = false;
}

void jdk::ClassSignatureVisitor::visitTypeArgument()
{
	buffer.append("<?");
	open++;
}

SignatureVisitorPointer jdk::ClassSignatureVisitor::visitTypeArgument(QChar& c)
{
	if (lastIsClass) {
		buffer.append("<");
		open++;
	}
	else {
		buffer.append(", ");
	}

	if (c == QChar('+')) {
		buffer.append("? extends ");
	}
	else if (c == QChar('-')) {
		buffer.append("? super ");
	}

	return SignatureVisitor::visitTypeArgument(c);
}

void jdk::ClassSignatureVisitor::visitEnd()
{
	if (open>0) {
		buffer.append(">");
		open--;
	}
}

void jdk::ClassSignatureVisitor::setFieldModel()
{
	if (s != Step::start) {
		throw JdkException("signature model error", FILE_INFO);
	}
	fieldModel = true;
}


jdk::ClassSignatureVisitor::ClassSignatureVisitor():s(Step::start), buffer("")
{

}

jdk::MethodSignatureVisitor::MethodSignatureVisitor():s(Step::start),returnOffset(-1), throwsOffset(-1)
{
}

void jdk::MethodSignatureVisitor::visitFormalTypeParameter(QString& name)
{
	if (s == Step::start) {
		s = Step::formal;
		buffer.append("<");
	}
	else if (s == Step::formal) {
		buffer.append(", ");
	}
	buffer.append(name);

}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitClassBound()
{
	buffer.append(" extends ");
	return SignatureVisitor::visitClassBound();
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitInterfaceBound()
{
	if (s == Step::formal) {
		buffer.append(" & ");
	}
	return SignatureVisitor::visitInterfaceBound();
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitParameterType()
{
	if (s == Step::formal) {
		s = Step::paramter;
		buffer.append(">");
		paramOffset = buffer.size();
	}
	else if(s == Step::start) {
		s = Step::paramter;
		paramOffset = 0;
	}
	else {
		buffer.append(", ");
	}
	
	return SignatureVisitor::visitParameterType();
}

void jdk::MethodSignatureVisitor::visitBaseType(QChar& b)
{
	buffer.append(Util::baseTypeName(b));
}

void jdk::MethodSignatureVisitor::visitTypeVariable(QString& name)
{
	buffer.append(name);
	lastIsClass = false;
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitArrayType()
{
	return SignatureVisitor::visitArrayType();
}

void jdk::MethodSignatureVisitor::visitClassType(QString& name)
{
	auto name2 = name.replace("/", ".");
	buffer.append(name2);
	lastIsClass = true;
}

void jdk::MethodSignatureVisitor::visitInnerClassType(QString& name)
{
	if (open > 0) {
		buffer.append(">");
		open--;
	}
	buffer.append(".");
	buffer.append(name);
	lastIsClass = true;
}

void jdk::MethodSignatureVisitor::visitTypeArgument()
{
	buffer.append("<?");
	open++;
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitTypeArgument(QChar& c)
{
	if (lastIsClass) {
		buffer.append("<");
		open++;
	}
	else {
		buffer.append(", ");
	}

	if (c == QChar('+')) {
		buffer.append("? extends ");
	}
	else if (c == QChar('-')) {
		buffer.append("? super ");
	}

	return SignatureVisitor::visitTypeArgument(c);
}

void jdk::MethodSignatureVisitor::visitEnd()
{
	if (open > 0) {
		buffer.append(">");
		open--;
	}
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitReturnType()
{
	if (s != Step::returnParamter) {
		s = Step::returnParamter;
		returnOffset = buffer.size();
	}
	return SignatureVisitor::visitReturnType();
}

SignatureVisitorPointer jdk::MethodSignatureVisitor::visitExceptionType()
{
	if (s != Step::throws) {
		s = Step::throws;
		throwsOffset = buffer.size();
	}
	else {
		buffer.append(", ");
	}
	return SignatureVisitor::visitExceptionType();
}

QString jdk::MethodSignatureVisitor::getRetureSignature()
{
	if (throwsOffset > 0) {
		return buffer.mid(returnOffset, throwsOffset - returnOffset);
	}
	else {
		return buffer.mid(returnOffset, buffer.size() - returnOffset);
	}
}

QString jdk::MethodSignatureVisitor::getParamSignature()
{
	return buffer.mid(paramOffset, returnOffset- paramOffset);
}

QString jdk::MethodSignatureVisitor::getThrowsSignature()
{
	if (throwsOffset > 0) {
		return buffer.mid(throwsOffset, buffer.size() - throwsOffset);
	}
	return QString();
}

QString jdk::MethodSignatureVisitor::getTypeParameters()
{
	if (paramOffset > 0) {
		return buffer.mid(0, paramOffset);
	}
	return QString();
}
