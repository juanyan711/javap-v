#include "stdafx.h"
#include "ClassFileModel.h"

ClassFileModel::ClassFileModel(std::shared_ptr<ClassFile>& cf, QObject* parent) :QAbstractListModel(parent), classFile(cf)
{
	//打印类的摘要信息
	printClassDigest();

	//显示常量池
	printConstantPool();

	//打印字段和方法
	printClassBody();

}

QString ClassFileModel::md5(QString fileName) const
{
	QFileInfo info(fileName);
	const uint64_t file_size = info.size();
	QCryptographicHash hash(QCryptographicHash::Md5);

	hash.addData(classFile->getBuffer(), file_size);
	return QString(hash.result().toHex());
}

QString ClassFileModel::modified(QString fileName) const
{
	QFileInfo info(fileName);
	return info.lastModified().toString("yyyy-MM-dd hh:mm:ss");
}

uint32_t ClassFileModel::fileSize(QString fileName) const
{
	QFileInfo info(fileName);
	return info.size();
}

QString ClassFileModel::getSourceFile() const
{
	return classFile->getSourceFile();
}

QString ClassFileModel::getClassFullName() const
{
	QString buffer("");

	AccessFlags flags(classFile->getAccessFlags());
	auto modifiers = flags.getClassModifiers();
	for (auto mod : modifiers) {
		buffer.append(mod);
		buffer.append(" ");
	}

	buffer.append("class ");

	buffer.append(classFile->getThisClass().replace(QChar('/'), QChar('.')));
	QString signature = classFile->getSignature();
	if (!signature.isEmpty()) {
		ClassSignatureVisitor csVisitor;
		SignatureReader sReader(signature);
		auto cVisitor = std::make_shared<ClassSignatureVisitor>();
		sReader.accept(cVisitor);
		buffer.append(cVisitor->toString());
	}
	return buffer;
}

QString ClassFileModel::getClassFlagsName() const
{
	AccessFlags flags(classFile->getAccessFlags());
	auto names = flags.getClassFlags();
	QString buffer("");
	for (auto name : names) {
		buffer.append(name);
		buffer.append(" ");
	}
	return buffer;
}


void ClassFileModel::printClassDigest()
{
	QString fileName = classFile->getFileName();
	auto cpool = classFile->getConstantPool();
	stringList << QString::fromUtf8(u8"classfile %1").arg(fileName);
	stringList << QString::fromUtf8(u8"  Last modified %1 ; size %2 bytes").arg(modified(fileName)).arg(fileSize(fileName));
	stringList << QString::fromUtf8(u8"  MD5 checksum %1").arg(md5(fileName));
	stringList << QString::fromUtf8(u8"  Compiled from %1").arg(classFile->getSourceFile());
	stringList << getClassFullName();
	stringList << QString::fromUtf8(u8"  minor version: %1").arg(classFile->getMinor());
	stringList << QString::fromUtf8(u8"  major version: %1").arg(classFile->getMajor());
	stringList << QString::fromUtf8(u8"  flags:  (0x%1) %2").arg(classFile->getAccessFlags(), 4, 16, QChar('0')).arg(getClassFlagsName());

	uint16_t index = classFile->getThisClassIndex();
	stringList << QString::fromUtf8(u8"  this_class: #%1 %2 // %3").arg(index).arg(" ", 30 - Util::digits(index)*2).arg(classFile->getThisClass());

	index = classFile->getSuperClassIndex();
	stringList << QString::fromUtf8(u8"  super_class: #%1 %2 // %3").arg(index).arg(" ", 27 - Util::digits(index)*2).arg(classFile->getSuperClass());

	stringList << QString::fromUtf8(u8"  interfaces:  %1, fields:  %2, methods:  %3, attributes:  %4").arg(classFile->getInterfaceCount()).arg(classFile->getFieldCount())
		.arg(classFile->getMethodCount()).arg(classFile->getAttributeCount());
}

void ClassFileModel::printConstantPool()
{
	auto cpool = classFile->getConstantPool();
	stringList << QString::fromUtf8(u8"Constant pool:");

	cpool->each([=](std::shared_ptr<CpInfo>& cpInfo, uint32_t index) {
		const int32_t SPACE = 18;
		auto tag = cpInfo->getTag();
		if (tag == ConstantTag::CONSTANT_UTF8) {
			stringList << QString::fromUtf8(u8"  #%1 = Utf8 %2 %3 ").arg(index).arg(" ", SPACE - Util::digits(index)*2).arg(cpool->getUtf8(index));
		}
		if (tag == ConstantTag::CONSTANT_CLASS) {
			ClassInfo* cInfo = reinterpret_cast<ClassInfo*>(cpInfo.get());
			stringList << QString::fromUtf8(u8"  #%1 = Class %2 #%3 %4 // %5 ").arg(index)
				                                                            .arg(" ", SPACE-2 - Util::digits(index) * 2)
				                                                            .arg(cInfo->getNameIndex())
				                                                            .arg(" ", 14 - Util::digits(cInfo->getNameIndex())*2)
				                                                            .arg(cpool->getClassName(index));
		} 
		if (tag == ConstantTag::CONSTANT_METHOD_REF) {
			MethodRefInfo* mrInfo = reinterpret_cast<MethodRefInfo*>(cpInfo.get());
			uint16_t classIndex = mrInfo->getClassIndex();
			uint16_t ntIndex = mrInfo->getNameAndTypeIndex();
			QString className = cpool->getClassName(classIndex);
			stringList << QString::fromUtf8(u8"  #%1 = Methodref %2 #%3.#%4 %5 // %6").arg(index)
				                                                              .arg(" ", SPACE - 10 - Util::digits(index) * 2)
				                                                              .arg(classIndex)
				                                                              .arg(ntIndex)
				                                                              .arg(" ", 12 - (Util::digits(classIndex) * 2 + Util::digits(ntIndex)*2))
				                                                              .arg(cpool->getMethodRef(index));
		}
	});
}

void ClassFileModel::printClassBody()
{
	stringList << QString::fromUtf8(u8"{");
	printFields();
	printMethods();
	stringList << QString::fromUtf8(u8"}");
}

void ClassFileModel::printFields()
{
	auto fields = classFile->getFields();
	for (auto field : fields) {

		QString buffer;
		buffer.append("  ");
		AccessFlags flags(field->getAccessFlags());
		auto modifiers = flags.getFieldModifiers();
		for (auto modifier : modifiers) {
			buffer.append(modifier);
			buffer.append(" ");
		}
		QString flagString;
		for (auto flag : flags.getFieldFlags()) { 
			flagString.append(flag);
			flagString.append("  ");
		}

		uint16_t sIndex = field->getSignatureIndex();
		QString signature = field->getSignature();
		QString type;
		if (!signature.isEmpty()) {
			SignatureReader sReader(signature);
			auto cVisitor = std::make_shared<ClassSignatureVisitor>();
			cVisitor->setFieldModel();
			sReader.accept(cVisitor);
			type = cVisitor->toString();
		}

		buffer.append(type);
		buffer.append(" ");
		buffer.append(field->getName());
		buffer.append(" ");
		
		stringList << buffer;
		stringList << QString::fromUtf8(u8"     descriptor: %1").arg(field->getDescriptor());
		stringList << QString::fromUtf8(u8"     flags: (0x%1)  %2").arg(field->getAccessFlags(), 4, 16, QChar('0')).arg(flagString);
		if (!signature.isEmpty()) {
			stringList << QString::fromUtf8(u8"     Signature: #%1 %2 //%3").arg(sIndex).arg("    ").arg(signature);
		}
		stringList << "";
	}
}

void ClassFileModel::printMethods()
{
	auto methods = classFile->getMethods();
	for (auto method : methods) {
		QString buffer;
		buffer.append("  ");
		AccessFlags flags(method->getAccessFlags());
		auto modifiers = flags.getMethodModifiers();
		for (auto modifier : modifiers) {
			buffer.append(modifier);
			buffer.append("  ");
		}
		QString flagString;
		for (auto flag : flags.getFieldFlags()) {
			flagString.append(flag);
			flagString.append(" ");
		}

		QString methodName = method->getName();
		if (QString::compare("<init>", methodName) == 0) {
			methodName = classFile->getThisClass().replace("/", ".");
			buffer.append(methodName);
			buffer.append("();");
		}
		else {
			uint16_t sIndex = method->getSignatureIndex();
			QString signature = method->getSignature();
			auto mVisitor = std::make_shared<MethodSignatureVisitor>();
			if (!signature.isEmpty()) {
				SignatureReader sReader(signature);
				sReader.accept(mVisitor);
				buffer.append(mVisitor->getRetureSignature());
				buffer.append("  ");
			}
			buffer.append(methodName);
			buffer.append("( ");
			buffer.append(mVisitor->getParamSignature());
			buffer.append(" ) ");

			if (!mVisitor->getThrowsSignature().isEmpty()) {
				buffer.append(" throws ");
				buffer.append(mVisitor->getThrowsSignature());
			}
		}

		stringList << buffer;
		stringList << QString::fromUtf8(u8"     descriptor: %1").arg(method->getDescriptor());
		stringList << QString::fromUtf8(u8"     flags: (0x%1)  %2").arg(method->getAccessFlags(), 4, 16, QChar('0')).arg(flagString);
		stringList << QString::fromUtf8(u8"     Code:");
		stringList << "";
	}
}

int ClassFileModel::rowCount(const QModelIndex& parent) const
{
	return stringList.count();
}

QVariant ClassFileModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if (index.row() >= stringList.size()) {
		return QVariant();
	}
	if (role == Qt::DisplayRole) {
		return stringList.at(index.row());
	}
	return QVariant();
}

QVariant ClassFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal) {
		return QString("Column %1").arg(section);
	}
	else {
		return QString("Row %1").arg(section);
	}
}


