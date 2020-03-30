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

void jdk::CpInfo::setConstantPool(std::shared_ptr<ConstantPool>& src)
{
	this->pool = std::weak_ptr<ConstantPool>(src);
}

const QString ClassInfo::OBJECT = QString("java/lang/Object");
ClassInfo::ClassInfo(uint16_t nameIndex, uint32_t off):CpInfo(ConstantTag::CONSTANT_CLASS, off), 
															nameIndex(nameIndex)
{
	length = 2;
}

QString jdk::ClassInfo::getClassName()
{
	auto spool = pool.lock();
	if (spool == nullptr) {
		return QString();
	}
	std::shared_ptr<CpInfo> info = spool->at(nameIndex);
	if (info->getTag() != ConstantTag::CONSTANT_UTF8) {
		qDebug() << "constant item tag is not utf8";
		throw JdkException("constant item tag fault", FILE_INFO);
	}
	Utf8Info* utf8 = reinterpret_cast<Utf8Info*>(info.get());
	return utf8->getString();
}

uint16_t jdk::ClassInfo::getNameIndex() const
{
	return this->nameIndex;
}

StringInfo::StringInfo(uint16_t string, uint32_t off):CpInfo(ConstantTag::CONSTANT_STRING, off), 
													  utf8Index(string)
{
	length = 2;
}

uint16_t StringInfo::getUtf8Index() {
	return utf8Index;
}

QString jdk::StringInfo::getString()
{
	auto spool = pool.lock();
	if (spool == nullptr) {
		return QString();
	}
	std::shared_ptr<CpInfo> info = spool->at(utf8Index);
	if (info->getTag() != ConstantTag::CONSTANT_UTF8) {
		qDebug() << "constant item tag is error";
		throw JdkException("constant item tag fault", FILE_INFO);
	}
	Utf8Info* utf8 = reinterpret_cast<Utf8Info*>(info.get());
	return QString::fromUtf8(utf8->getByteArray().constData());
}



NameAndTypeInfo::NameAndTypeInfo(uint16_t n, uint16_t d, uint32_t o):CpInfo(ConstantTag::CONSTANT_NAME_AND_TYPE, o),
																			nameIndex(n), 
																			descriptIndex(d)
{
	length = 4;
}

uint16_t jdk::NameAndTypeInfo::getNameIndex()
{
	return nameIndex;
}

uint16_t jdk::NameAndTypeInfo::getDescriptIndex()
{
	return descriptIndex;
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

QString jdk::Utf8Info::getString()
{
	QString str = QString::fromUtf8(bytes, utf8ByteLength);
	return str;
}

QByteArray jdk::Utf8Info::getByteArray()
{
	return bytes;
}

MethodHandleInfo::MethodHandleInfo(ReferenceKind rk, uint16_t ri, uint32_t o):CpInfo(ConstantTag::CONSTANT_METHOD_HANDLE, o),
																					kind(rk), 
																					referenceIndex(ri)
{
	length = 3;
}

MethodTypeInfo::MethodTypeInfo(uint16_t d, uint32_t o):CpInfo(ConstantTag::CONSTANT_METHOD_TYPE, o), 
															descriptorIndex(d)
{
	length = 2;
}

InvokeDynamicInfo::InvokeDynamicInfo(uint16_t b, uint16_t n, uint32_t o):CpInfo(ConstantTag::CONSTANT_INVOKE_DYNAMIC, o),
																			bootstrapMethodAttrIndex(b),
																			nameAndTypeIndex(n)
{
	length = 4;
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
	long high = Util::readUnsignedInt32(b, offset);
	long low = Util::readUnsignedInt32(b, offset + 4) & 0xFFFFFFFFL;
	if (tag == ConstantTag::CONSTANT_LONG) {
		return std::unique_ptr<CpInfo>(new LongInfo(high, low, offset));
	}
	if (tag == ConstantTag::CONSTANT_DOUBLE) {
		return std::unique_ptr<CpInfo>(new DoubleInfo(high, low, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::NameAndTypeInfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint16_t nameIndex = Util::readUnsignedInt16(b, offset);
	uint16_t descIndex = Util::readUnsignedInt16(b, offset+2);
	if (tag == ConstantTag::CONSTANT_NAME_AND_TYPE) {
		return std::unique_ptr<CpInfo>(new NameAndTypeInfo(nameIndex, descIndex, offset));
	}
	return std::unique_ptr<CpInfo>(nullptr);
}

std::unique_ptr<CpInfo> jdk::MethodHandleInfoFactory::createCpInfo(uint8_t tag, const char* b, uint32_t offset)
{
	uint8_t kind = b[offset];
	uint16_t referenceIndex = Util::readUnsignedInt16(b, offset + 1);
	if (tag == ConstantTag::CONSTANT_METHOD_HANDLE) {
		return std::unique_ptr<CpInfo>(new MethodHandleInfo((ReferenceKind)kind, referenceIndex, offset));
	}
	return std::unique_ptr<CpInfo>();
}

std::vector<std::shared_ptr<ICpInfoFactory>> ConstantPool::cpInfoFactorys = []() {
	std::vector<std::shared_ptr<ICpInfoFactory>> v;
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U2InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new RefAndDynamicInfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U4InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new U8InfoFactory()));
	v.push_back(std::shared_ptr<ICpInfoFactory>(new NameAndTypeInfoFactory()));
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

QString jdk::ConstantPool::getClassName(uint16_t classIndex) const
{
	std::shared_ptr<CpInfo> cpInfo = this->at(classIndex);
	if (cpInfo->getTag() != ConstantTag::CONSTANT_CLASS) {
		qDebug() << "constant item tag is error";
		throw JdkException("constant item tag is not class", FILE_INFO);
	}
	ClassInfo* ci = reinterpret_cast<ClassInfo*>(cpInfo.get());
	return ci->getClassName();
}

QString jdk::ConstantPool::getUtf8(uint16_t utf8Index) const
{
	std::shared_ptr<CpInfo> cp = this->at(utf8Index);
	if (cp->getTag() != ConstantTag::CONSTANT_UTF8) {
		qDebug() << "attribute name index is fault";
		throw JdkException("attribute name index is fault", FILE_INFO);
	}
	Utf8Info* utf8 = reinterpret_cast<Utf8Info*>(cp.get());
	return utf8->getString();
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
		cpInfo->setConstantPool(pool);
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

	return std::move(pool);
}