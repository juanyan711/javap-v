#include "stdafx.h"
#include "constantPool.h"

using namespace jdk;

CpInfo::CpInfo(uint8_t p):tag(p), offset(-1)
{
}

CpInfo::CpInfo(uint8_t t, uint32_t off):tag(t), offset(off)
{
}

CpInfo::~CpInfo()
{
}

uint8_t CpInfo::getTag()
{
	return tag;
}

void CpInfo::setOffset(uint32_t off)
{
	this->offset = off;
}

uint32_t CpInfo::getOffset()
{
	return this->offset;
}

uint32_t jdk::CpInfo::getLength()
{
	return this->length;
}

void jdk::CpInfo::setConstantPool(const ConstantPool* src)
{
	this->pool = src;
}

const QString ClassInfo::OBJECT = QString("java/lang/Object");
ClassInfo::ClassInfo(uint16_t i, uint32_t off):SimpleRefInfo(i, off, ConstantTag::CONSTANT_CLASS)
{
}

QString jdk::ClassInfo::toString() const
{
	QString name = SimpleRefInfo::toString();
	return name;
}

StringInfo::StringInfo(uint16_t i, uint32_t off):SimpleRefInfo(i, off, ConstantTag::CONSTANT_STRING)
{
}

QString jdk::StringInfo::toString() const
{
	return QString::fromLocal8Bit("\"%1\"").arg(SimpleRefInfo::toString());
}

NameAndTypeInfo::NameAndTypeInfo(uint16_t n, uint16_t d, uint32_t o):BaseRefInfo(n,d, o, ConstantTag::CONSTANT_NAME_AND_TYPE)
{
}

uint16_t jdk::NameAndTypeInfo::getNameIndex() const
{
	return getIndex1();
}

uint16_t jdk::NameAndTypeInfo::getDescriptIndex() const
{
	return getIndex2();
}

QString jdk::NameAndTypeInfo::toString() const
{
	QString buffer;
	auto name = pool->at(index1);
	auto desc = pool->at(index2);
	if (name->getTag() != ConstantTag::CONSTANT_UTF8 || desc->getTag() != ConstantTag::CONSTANT_UTF8) {
		throw JdkException("tag is not utf8", FILE_INFO);    //不要直接调用toString，避免无限迭代
	}
	buffer.append(name->toString());
	buffer.append(":");
	buffer.append(desc->toString());
	return buffer;
}

std::unique_ptr<CpInfo> jdk::Utf8InfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint16_t len = Util::readUnsignedInt16(b, offset);
	if (tag == ConstantTag::CONSTANT_UTF8) {
		return std::unique_ptr<CpInfo>(new Utf8Info(b + offset + 2, len, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

jdk::Utf8Info::Utf8Info(const char* b, uint16_t len, uint32_t o):CpInfo(ConstantTag::CONSTANT_UTF8, o), bytes(b), utf8ByteLength(len)
{
	length = 2 + utf8ByteLength;
	if (utf8ByteLength > 0xFFFF) {
		qDebug() << "warning:string(utf8) too long";
	}
}

uint32_t Utf8Info::getByteLength()
{
	return utf8ByteLength;
}

QString jdk::Utf8Info::toString() const
{
	QString str = QString::fromUtf8(bytes, utf8ByteLength);
	return str;
}

QByteArray jdk::Utf8Info::getByteArray()
{
	return bytes;
}

MethodHandleInfo::MethodHandleInfo(uint8_t rk, uint16_t ri, uint32_t o):CpInfo(ConstantTag::CONSTANT_METHOD_HANDLE, o),
																					kind(rk), 
																					referenceIndex(ri)
{
	length = 3;
}

uint16_t jdk::MethodHandleInfo::getReferenceKind() const
{
	return (uint16_t)kind;
}

uint16_t jdk::MethodHandleInfo::getReferenceIndex() const
{
	return referenceIndex;
}

QString jdk::MethodHandleInfo::toString() const
{
	QString kindname;
	QString referencevalue;
	switch (kind) {
	case 1:
		kindname = "REF_getField";
		referencevalue = pool->getFieldRef(referenceIndex);
		break;
	case 2:
		kindname = "REF_getStatic";
		referencevalue = pool->getFieldRef(referenceIndex);
		break;
	case 3:
		kindname = "REF_putField";
		referencevalue = pool->getFieldRef(referenceIndex);
		break;
	case 4:
		kindname = "REF_putStatic";
		referencevalue = pool->getFieldRef(referenceIndex);
		break;
	case 5:
		kindname = "REF_invokeVirtual";
		referencevalue = pool->getMethodRef(referenceIndex);
		break;
	case 6:
		kindname = "REF_invokeStatic";
		referencevalue = pool->getGeneral(referenceIndex);
		break;
	case 7:
		kindname = "REF_invokeSpecial";
		referencevalue = pool->getGeneral(referenceIndex);
		break;
	case 8:
		kindname = "REF_newInvokeSpecial";
		referencevalue = pool->getMethodRef(referenceIndex);
		break;
	case 9:
		kindname = "REF_invokeInterface";
		referencevalue = pool->getGeneral(referenceIndex);
		break;
	default:
		kindname = "unkown";
		referencevalue = "error";
		break;
	}
	return QString().append(kindname).append(" ").append(referencevalue);
}

MethodTypeInfo::MethodTypeInfo(uint16_t i, uint32_t o):SimpleRefInfo(i, o, ConstantTag::CONSTANT_METHOD_TYPE)
{
}

InvokeDynamicInfo::InvokeDynamicInfo(uint16_t n, uint16_t d, uint32_t o): BaseRefInfo(n, d, o, ConstantTag::CONSTANT_INVOKE_DYNAMIC)
{
	length = 4;
}



uint16_t jdk::InvokeDynamicInfo::getBootstrapMethodAttrIndex() const
{
	return getIndex1();
}

uint16_t jdk::InvokeDynamicInfo::getNameAndTypeIndex() const
{
	return getIndex2();
}

QString jdk::InvokeDynamicInfo::toString() const
{
	uint16_t natIndex = this->getNameAndTypeIndex();
	return QString::fromLocal8Bit("#%1:%2").arg(this->getBootstrapMethodAttrIndex()).arg(pool->getNameAndType(natIndex));
}


std::unique_ptr<CpInfo> jdk::U2InfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint16_t index = Util::readUnsignedInt16(b, offset);
	if (tag == ConstantTag::CONSTANT_CLASS) {
		return std::unique_ptr<CpInfo>(new ClassInfo(index, offset));
	}
	if (tag == ConstantTag::CONSTANT_STRING) {
		return std::unique_ptr<CpInfo>(new StringInfo(index, offset));
	}
	if (tag == ConstantTag::CONSTANT_METHOD_TYPE) {
		return std::unique_ptr<CpInfo>(new MethodTypeInfo(index, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::RefAndDynamicInfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint16_t index1 = Util::readUnsignedInt16(b, offset);
	uint16_t index2 = Util::readUnsignedInt16(b, offset + 2);
	if (tag == ConstantTag::CONSTANT_FIELD_REF) {
		return std::unique_ptr<CpInfo>(new FieldRefInfo(index1, index2, offset));
	}
	if (tag == ConstantTag::CONSTANT_METHOD_REF) {
		return std::unique_ptr<CpInfo>(new MethodRefInfo(index1, index2, offset));
	}
	if (tag == ConstantTag::CONSTANT_INTERFACE_METHOD_REF) {
		return std::unique_ptr<CpInfo>(new InterfaceRefInfo(index1, index2, offset));
	}
	if (tag == ConstantTag::CONSTANT_INVOKE_DYNAMIC) {
		return std::unique_ptr<CpInfo>(new InvokeDynamicInfo(index1, index2, offset));
	}
	if (tag == ConstantTag::CONSTANT_NAME_AND_TYPE) {
		return std::unique_ptr<CpInfo>(new NameAndTypeInfo(index1, index2, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::U4InfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	//参考:Float.c Java_java_lang_Float_intBitsToFloat
	union {
		uint32_t i;
		float f;
	} u;
	u.i = Util::readUnsignedInt32(b, offset);
	if (tag == ConstantTag::CONSTANT_INTEGER) {
		return std::unique_ptr<CpInfo>(new IntegerInfo(u.i, offset));
	}
	if (tag == ConstantTag::CONSTANT_FLOAT) {
		return std::unique_ptr<CpInfo>(new FloatInfo(u.f, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::U8InfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint32_t high = Util::readUnsignedInt32(b, offset);
	uint32_t low = Util::readUnsignedInt32(b, offset + 4);
	union {
		int64_t l;
		double d;
	} v;
	v.l = ((uint64_t)high << 32) | ((uint64_t)low);
	if (tag == ConstantTag::CONSTANT_LONG) {
		return std::unique_ptr<CpInfo>(new LongInfo(high, low, offset, v.l));
	}
	if (tag == ConstantTag::CONSTANT_DOUBLE) {
		return std::unique_ptr<CpInfo>(new DoubleInfo(high, low, offset, v.d));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::MethodHandleInfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint8_t kind = b[offset];
	uint16_t referenceIndex = Util::readUnsignedInt16(b, offset + 1);
	if (tag == ConstantTag::CONSTANT_METHOD_HANDLE) {
		return std::unique_ptr<CpInfo>(new MethodHandleInfo(kind, referenceIndex, offset));
	}
	return std::unique_ptr<CpInfo>();
}

std::vector<std::shared_ptr<ICpInfoFactory>> ConstantPool::cpInfoFactorys = []() {
	std::vector<std::shared_ptr<ICpInfoFactory>> v;
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U2InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new RefAndDynamicInfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U4InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U8InfoFactory()));
	//v.push_back(std::shared_ptr<ICpInfoFactory>(new NameAndTypeInfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new Utf8InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new MethodHandleInfoFactory()));
	return std::move(v);
}();

ConstantPool::ConstantPool(uint16_t poolSize, uint32_t os) :cpInfoSize(poolSize), offset(os), length(0) {
	pool.reserve(cpInfoSize); //一次性的预留好空间
}

void ConstantPool::add(const std::shared_ptr<CpInfo>& cpInfo)
{
	if (cpInfoSize < 1) {
		qDebug() << "error: did not read length";
		throw JdkException("did not read length", FILE_INFO);
	}
	if (pool.size() > cpInfoSize) {
		qDebug() << "error: too many element";
		throw JdkException("too many element", FILE_INFO);
	}
	if (pool.size() == 0) {
		qDebug() << "init index =0 in constant pool";
		pool.push_back(std::shared_ptr<CpInfo>(nullptr));
	}
	pool.push_back(cpInfo);
}

std::shared_ptr<CpInfo> ConstantPool::at(uint32_t index) const
{
	if (index > pool.size() || index < 1) {
		qDebug() << "error: index is: " << index << ", pool size is: " << pool.size();
		throw JdkException("index error", FILE_INFO);
	}
	return pool[index];
}

uint32_t jdk::ConstantPool::getLength() const
{
	return this->length;
}

uint16_t jdk::ConstantPool::getConstantItemNumber() const
{
	return this->cpInfoSize;
}

QString jdk::ConstantPool::getClassName(uint16_t classIndex) const
{
	std::shared_ptr<CpInfo> cpInfo = this->at(classIndex);
	if (cpInfo->getTag() != ConstantTag::CONSTANT_CLASS) {
		qDebug() << "constant item tag is error";
		throw JdkException("constant item tag is not class", FILE_INFO);
	}
	ClassInfo* ci = reinterpret_cast<ClassInfo*>(cpInfo.get());
	return ci->toString();
}

QString jdk::ConstantPool::getUtf8(uint16_t utf8Index) const
{
	std::shared_ptr<CpInfo> cp = this->at(utf8Index);
	if (cp->getTag() != ConstantTag::CONSTANT_UTF8) {
		qDebug() << "attribute name index is fault";
		throw JdkException("attribute name index is fault", FILE_INFO);
	}
	Utf8Info* utf8 = reinterpret_cast<Utf8Info*>(cp.get());
	return utf8->toString();
}

QString jdk::ConstantPool::getNameAndType(uint16_t ntIndex) const
{
	std::shared_ptr<CpInfo> cp = this->at(ntIndex);
	if (cp->getTag() != ConstantTag::CONSTANT_NAME_AND_TYPE) {
		qDebug() << "nameAndTypeindex is fault";
		throw JdkException("attribute name index is fault", FILE_INFO);
	}
	NameAndTypeInfo* nt = reinterpret_cast<NameAndTypeInfo*>(cp.get());
	QString buffer;
	buffer.append("\"");
	buffer.append(getUtf8(nt->getNameIndex()));
	buffer.append("\"");
	buffer.append(":");
	buffer.append(getUtf8(nt->getDescriptIndex()));

	return buffer;
}

QString jdk::ConstantPool::getMethodRef(uint16_t mrIndex) const
{
	std::shared_ptr<CpInfo> cp = this->at(mrIndex);
	if (cp->getTag() != ConstantTag::CONSTANT_METHOD_REF) {
		qDebug() << "method ref index is fault";
		throw JdkException("method ref index is fault", FILE_INFO);
	}
	MethodRefInfo* mrInfo = reinterpret_cast<MethodRefInfo*>(cp.get());

	QString buffer;
	buffer.append(getClassName(mrInfo->getClassIndex()));
	buffer.append(".");
	buffer.append(getNameAndType(mrInfo->getNameAndTypeIndex()));
	return buffer;
}

QString jdk::ConstantPool::getFieldRef(uint16_t fieldIndex) const
{
	std::shared_ptr<CpInfo> cp = this->at(fieldIndex);
	if (cp->getTag() != ConstantTag::CONSTANT_FIELD_REF) {
		qDebug() << "method ref index is fault";
		throw JdkException("method ref index is fault", FILE_INFO);
	}
	FieldRefInfo* mrInfo = reinterpret_cast<FieldRefInfo*>(cp.get());

	QString buffer;
	buffer.append(getClassName(mrInfo->getClassIndex()));
	buffer.append(".");
	buffer.append(getNameAndType(mrInfo->getNameAndTypeIndex()));
	return buffer;
}

QString jdk::ConstantPool::getGeneral(uint16_t index) const
{
	std::shared_ptr<CpInfo> cp = this->at(index);
	return cp->toString();
}

std::shared_ptr<ConstantPool> jdk::ConstantPool::createConstantPool(QByteArray& bytes, uint32_t offset)
{
	//常量池内容
	const char* buffer = bytes.constData();
	uint16_t constantPoolCount = Util::readUnsignedInt16(buffer, offset);
	qDebug() << "constpool size:" << constantPoolCount;

	//构建一个常量池对象
	auto pool = std::shared_ptr<ConstantPool>(new ConstantPool(constantPoolCount, offset));

	//开始分析常量数组
	uint32_t currentCpInfoIndex = 1;
	uint32_t currentCpInfoOffset = offset + 2;   //去掉constantPool的长度值
	uint32_t constPoolSize = 0;
	while (currentCpInfoIndex < constantPoolCount) {
		uint8_t tag = buffer[currentCpInfoOffset];
		//qDebug() << "index:" << currentCpInfoIndex << " , tag:" << tag;
		auto cpInfoOffset = currentCpInfoOffset + 1;  //跳过最开始的一个常量类型字段
		uint32_t cpInfoSize = 0;
		std::shared_ptr<CpInfo> cpInfo;
		for (auto ifactory : cpInfoFactorys) {
			cpInfo = ifactory->createCpInfo(tag, buffer, cpInfoOffset);
			if (cpInfo.get() != nullptr) {
				break;
			}
		}
		if (cpInfo.get() == nullptr) {
			qDebug() << "unkown constant tag, tag is: "<<tag << " , cpinfo index :" << currentCpInfoIndex <<" ,offset:"<< currentCpInfoOffset;
			throw JdkException("unkown constant tag", FILE_INFO);
		}
		pool->add(cpInfo);
		cpInfo->setConstantPool(pool.get());
		if (cpInfo->getTag() == ConstantTag::CONSTANT_LONG || cpInfo->getTag() == ConstantTag::CONSTANT_DOUBLE) {
			currentCpInfoIndex++;
			pool->add(std::unique_ptr<CpInfo>(nullptr));
		}

		cpInfoSize = cpInfo->getLength() + 1;
		constPoolSize += cpInfoSize;
		currentCpInfoOffset += cpInfoSize;
		
		currentCpInfoIndex++;
	}
	pool->length = constPoolSize + 2;   //加上开始的两个字节

	return pool;
}

QString jdk::ConstantTag::translate(uint8_t t, bool constant)
{
	QString buffer;
	switch (t) {
	case CONSTANT_CLASS:
		buffer.append(constant ? "Class":"class");
		break;
	case CONSTANT_FIELD_REF:
		buffer.append(constant ? "FieldRef" : "Field");
		break;
	case CONSTANT_METHOD_REF:
		buffer.append(constant ? "MethodRef" : "Method");
		break;
	case CONSTANT_INTERFACE_METHOD_REF:
		buffer.append(constant ? "InterfaceMethodRef" : "InterfaceMethod");
		break;
	case CONSTANT_STRING:
		buffer.append("String");
		break;
	case CONSTANT_INTEGER:
		buffer.append(constant ? "Integer":"int");
		break;
	case CONSTANT_FLOAT:
		buffer.append(constant ? "Float":"float");
		break;
	case CONSTANT_LONG:
		buffer.append(constant ? "Long":"long");
		break;
	case CONSTANT_DOUBLE:
		buffer.append(constant ? "Double":"double");
		break;
	case CONSTANT_NAME_AND_TYPE:
		buffer.append("NameAndType");
		break;
	case CONSTANT_UTF8:
		buffer.append("Utf8");
		break;
	case CONSTANT_METHOD_HANDLE:
		buffer.append("MethodHandle");
		break;
	case CONSTANT_METHOD_TYPE:
		buffer.append("MethodType");
		break;
	case CONSTANT_INVOKE_DYNAMIC:
		buffer.append("InvokeDynamic");
		break;
	default:
		break;
	}
	return buffer;
}

jdk::SimpleRefInfo::SimpleRefInfo(uint16_t i, uint32_t off, uint16_t tag) :CpInfo(tag, off), index(i) {
	length = 2;
}

uint16_t jdk::SimpleRefInfo::getIndex() const
{
	return index;
}

QString jdk::SimpleRefInfo::toString() const
{
	if (pool == nullptr) {
		return QString();
	}
	std::shared_ptr<CpInfo> info = pool->at(index);
	if (info->getTag() != ConstantTag::CONSTANT_UTF8) {
		qDebug() << "constant item tag is not utf8";
		throw JdkException("constant item tag fault", FILE_INFO);
	}
	Utf8Info* utf8 = reinterpret_cast<Utf8Info*>(info.get());
	return utf8->toString();
}

jdk::BaseRefInfo::BaseRefInfo(uint16_t cls, uint16_t nameAndType, uint32_t off, uint16_t tag) :CpInfo(tag, off),
                                                                                                   index1(cls), 
	                                                                                               index2(nameAndType)
{
	length = 4;
}

uint16_t jdk::BaseRefInfo::getIndex1() const
{
	return index1;
}

uint16_t jdk::BaseRefInfo::getIndex2() const
{
	return index2;
}

jdk::InsiderRefInfo::InsiderRefInfo(uint16_t cls, uint16_t nameAndType, uint32_t off, uint16_t tag):BaseRefInfo(cls, nameAndType, off, tag)
{
}

uint16_t jdk::InsiderRefInfo::getClassIndex()
{
	return getIndex1();
}

uint16_t jdk::InsiderRefInfo::getNameAndTypeIndex()
{
	return getIndex2();
}

QString jdk::BaseRefInfo::toString() const
{
	QString buffer;
	buffer.append(pool->getClassName(index1));
	buffer.append(".");
	buffer.append(pool->getNameAndType(index2));
	return buffer;
}

jdk::FieldRefInfo::FieldRefInfo(uint16_t ci, uint16_t ni, uint32_t off):InsiderRefInfo(ci, ni, off, ConstantTag::CONSTANT_FIELD_REF)
{
}

jdk::MethodRefInfo::MethodRefInfo(uint16_t ci, uint16_t ni, uint32_t off): InsiderRefInfo(ci, ni, off, ConstantTag::CONSTANT_METHOD_REF)
{
}

jdk::InterfaceRefInfo::InterfaceRefInfo(uint16_t ci, uint16_t ni, uint32_t off) : InsiderRefInfo(ci, ni, off, ConstantTag::CONSTANT_INTERFACE_METHOD_REF)
{
}
