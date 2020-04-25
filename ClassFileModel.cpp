#include "stdafx.h"
#include "ClassFileModel.h"
#include "typeannotation.h"

ClassFileModel::ClassFileModel(const ClassFile* cf, QObject* parent) :classFile(cf), QAbstractListModel(parent)
{
}

void ClassFileModel::resetNodeType(NodeType type, void* data)
{
	if (type == NodeType::Inherit) {
		
	}
	if (items.size() > 0) {
		beginRemoveRows(QModelIndex(), 0, items.size() - 1);
		vector<shared_ptr<ListItem>>().swap(items);
		endRemoveRows();
	}
	if (classFile == nullptr) {
		return;
	}
	if (type == NodeType::Fields || type == NodeType::Methods) {
		return;
	}
	if (type == NodeType::Digest) {
		printClassDigest();
	}
	if (type == NodeType::Magic) {
		items.push_back(createItemNode(classFile->getMagicValue()));
	}
	if (type == NodeType::Version) {
		printVersion();
	}
	if (type == NodeType::ConstantPool) {
		printConstantPool();
	}
	if (type == NodeType::AccessFlags) {
		printAccessFlags();
	}
	if (type == NodeType::ThisClass) {
		printThisClass();
	}
	if (type == NodeType::SuperClass) {
		printSuperClass();
	}
	if (type == NodeType::Interfaces) {
		printInterfaces();
	}
	if (type == NodeType::Field) {
		printField(static_cast<FieldInfo*>(data));
	}
	if (type == NodeType::Method) {
		printMethod(static_cast<MethodInfo*>(data));
	}
	if (type == NodeType::ClassAttributes) {
		printClassAttributeInfo();
	}
	if (items.size() > 0) {
		beginInsertRows(QModelIndex(), 0, items.size()-1);
		endInsertRows();
	}
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

shared_ptr<ListItem> ClassFileModel::createItemNode(QString text)
{
	shared_ptr<ListItem> item(new ListItem);
	item->text = text;
	return item;
}

void ClassFileModel::printClassDigest()
{
	QString fileName = classFile->getFileName();
	auto cpool = classFile->getConstantPool();
	items.push_back(createItemNode(QString::fromUtf8(u8"classfile %1").arg(fileName)));
	items.push_back(createItemNode(QString::fromUtf8(u8"  Last modified %1 ; size %2 bytes").arg(modified(fileName)).arg(fileSize(fileName))));
	items.push_back(createItemNode(QString::fromUtf8(u8"  MD5 checksum %1").arg(md5(fileName))));
	items.push_back(createItemNode(QString::fromUtf8(u8"  Compiled from %1").arg(classFile->getSourceFile())));
	items.push_back(createItemNode(getClassFullName()));
	items.push_back(createItemNode(QString::fromUtf8(u8"  interfaces:  %1, fields:  %2, methods:  %3, attributes:  %4").arg(classFile->getInterfaceCount()).arg(classFile->getFieldCount())
		.arg(classFile->getMethodCount()).arg(classFile->getAttributeCount())));
}

void ClassFileModel::printVersion()
{
	items.push_back(createItemNode(QString::fromUtf8(u8"  minor version: %1").arg(classFile->getMinor())));
	items.push_back(createItemNode(QString::fromUtf8(u8"  major version: %1").arg(classFile->getMajor())));
}

void ClassFileModel::printConstantPool()
{
	auto cpool = classFile->getConstantPool();
	items.push_back(createItemNode(QString::fromUtf8(u8"Constant pool, length: %1").arg(classFile->getConstantPool()->getConstantItemNumber())));

	cpool->each([=](std::shared_ptr<CpInfo>& cpInfo, uint32_t index) {
		const int32_t SPACE = 18;
		auto tag = cpInfo->getTag();
		auto type = ConstantTag::translate(tag);
		if (tag == ConstantTag::CONSTANT_UTF8 || tag == ConstantTag::CONSTANT_INTEGER || tag == ConstantTag::CONSTANT_FLOAT ||
			tag == ConstantTag::CONSTANT_DOUBLE || tag == ConstantTag::CONSTANT_LONG) {
			items.push_back(createItemNode( QString::fromUtf8(u8"  #%1 = %2 %3 %4 ")
				.arg(index)
				.arg(type)
				.arg(" ", SPACE - Util::digits(index) * 2 )
				.arg(cpInfo->toString()) ));
			return;
		}
		if (tag == ConstantTag::CONSTANT_CLASS || tag == ConstantTag::CONSTANT_STRING 
			|| tag == ConstantTag::CONSTANT_METHOD_TYPE) {   
			SimpleRefInfo* sInfo = reinterpret_cast<SimpleRefInfo*>(cpInfo.get());
			items.push_back(createItemNode( QString::fromUtf8(u8"  #%1 = %2 %3 #%4 %5 // %6 ").arg(index)
											                                .arg(type)
				                                                            .arg(" ", SPACE-2 - Util::digits(index) * 2)
				                                                            .arg(sInfo->getIndex())
				                                                            .arg(" ", 14 - Util::digits(sInfo->getIndex())*2)
				                                                            .arg(sInfo->toString()) ));
			return;
		} 
		if (tag == ConstantTag::CONSTANT_METHOD_REF || tag == ConstantTag::CONSTANT_FIELD_REF ||
			tag == ConstantTag::CONSTANT_INTERFACE_METHOD_REF || tag == ConstantTag::CONSTANT_NAME_AND_TYPE ||
			tag == ConstantTag::CONSTANT_INVOKE_DYNAMIC || tag == ConstantTag::CONSTANT_METHOD_HANDLE) {
			uint16_t index1;
			uint16_t index2;
			if (tag == ConstantTag::CONSTANT_METHOD_HANDLE) {
				MethodHandleInfo* mrInfo = reinterpret_cast<MethodHandleInfo*>(cpInfo.get());
				index1 = mrInfo->getReferenceKind();
				index2 = mrInfo->getReferenceIndex();
			}
			else {
				BaseRefInfo* mrInfo = reinterpret_cast<BaseRefInfo*>(cpInfo.get());
				index1 = mrInfo->getIndex1();
				index2 = mrInfo->getIndex2();
			}
			QString sperator = (tag == ConstantTag::CONSTANT_NAME_AND_TYPE ||
								tag == ConstantTag::CONSTANT_INVOKE_DYNAMIC ||
				                tag == ConstantTag::CONSTANT_METHOD_HANDLE) ? ":" : ".";
			int32_t spaceLength = tag == ConstantTag::CONSTANT_METHOD_REF ? 10 : 5;
			QString four = (tag == ConstantTag::CONSTANT_METHOD_HANDLE) ? " " : "#";
			items.push_back(createItemNode( QString::fromUtf8(u8"  #%1 = %2 %3 %4%5%6#%7 %8 // %9")
				                                                              .arg(index)
																			  .arg(type)
				                                                              .arg(" ", SPACE - spaceLength - Util::digits(index) * 2)
				                                                              .arg(four)
				                                                              .arg(index1)
				                                                              .arg(sperator)
				                                                              .arg(index2)
				                                                              .arg(" ", 12 - (Util::digits(index1) * 2 + Util::digits(index2)*2))
				                                                              .arg(cpInfo->toString()) 
																			  
			                          ));
			return;
		}

	});
}

void ClassFileModel::printAccessFlags()
{
	items.push_back(createItemNode( QString::fromUtf8(u8"  flags:  (0x%1) %2").arg(classFile->getAccessFlags(), 4, 16, QChar('0')).arg(getClassFlagsName()) ));
}

void ClassFileModel::printThisClass()
{
	uint16_t index = classFile->getThisClassIndex();
	items.push_back(createItemNode( QString::fromUtf8(u8"  this_class: #%1 %2 // %3").arg(index).arg(" ", 30 - Util::digits(index) * 2).arg(classFile->getThisClass()) ));
}

void ClassFileModel::printSuperClass()
{
	uint16_t index = classFile->getSuperClassIndex();
	items.push_back(createItemNode( QString::fromUtf8(u8"  super_class: #%1 %2 // %3")
		.arg(index)
		.arg(" ", 27 - Util::digits(index) * 2)
		.arg(classFile->getSuperClass()) ));
}

void ClassFileModel::printInterfaces()
{
}

void ClassFileModel::printFields()
{
	auto fields = classFile->getFields();
	for (auto field : fields) {

		
	}
}

void ClassFileModel::printField(FieldInfo* fieldInfo)
{
	QString buffer;
	buffer.append("  ");
	AccessFlags flags(fieldInfo->getAccessFlags());
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

	QString signature = fieldInfo->getSignature();
	QString descriptor = fieldInfo->getDescriptor();
	QString type;
	if (!signature.isEmpty()) {
		SignatureReader sReader(signature);
		auto cVisitor = std::make_shared<ClassSignatureVisitor>();
		cVisitor->setFieldModel();
		sReader.accept(cVisitor);
		type = cVisitor->toString();
	}
	else {
		//若是没有signature,就用descript
		bool isArray = false;
		QString temp = descriptor;
		if (temp.startsWith("[")) {
			isArray = true;
			temp = temp.mid(1, temp.size() - 1);
		}
		if (temp.startsWith("L")) {
			type = temp.mid(1, temp.size() - 2).replace("/", ".");
		}
		else {
			type = Util::baseTypeName(temp.at(0));
		}

		if (isArray) {
			isArray = false;
			type += "[]";
		}
	}

	buffer.append(type);
	buffer.append(" ");
	buffer.append(fieldInfo->getName());
	buffer.append(" ");

	items.push_back(createItemNode(buffer));
	items.push_back(createItemNode( QString::fromUtf8(u8"     descriptor: %1").arg(descriptor) ));
	items.push_back(createItemNode(QString::fromUtf8(u8"     flags: (0x%1)  %2")
		.arg(fieldInfo->getAccessFlags(), 4, 16, QChar('0'))
		.arg(flagString)));
	items.push_back(createItemNode(""));
	if (!signature.isEmpty()) {
		uint16_t sIndex = fieldInfo->getSignatureIndex();
		items.push_back(createItemNode( QString::fromUtf8(u8"     Signature: #%1 %2 //%3")
			.arg(sIndex)
			.arg("    ")
			.arg(signature) ));
	}

	auto& attributes = fieldInfo->getAttributesRef();
	for (auto attribute : attributes) {
		if (QString::compare("Signature", attribute->getAttributeName()) != 0) {
			printAttributeInfo(attribute.get());
		}
	}
}

void ClassFileModel::printMethods()
{
	auto methods = classFile->getMethods();
	
	for (auto method : methods) {
		
	}
}

void ClassFileModel::printMethod(MethodInfo* method)
{
	QString buffer;
	buffer.append("  ");
	auto cpool = classFile->getConstantPool();
	AccessFlags flags(method->getAccessFlags());
	auto modifiers = flags.getMethodModifiers();
	auto itr = modifiers.crbegin();
	while (true) {
		if (itr == modifiers.crend()) {
			break;
		}
		buffer.append(*itr);
		buffer.append("  ");
		++itr;
	}
	QString flagString;
	auto strFlags = flags.getMethodFlags();
	auto fitr = strFlags.crbegin();
	while(true){
		if (fitr == strFlags.crend()) {
			break;
		}
		flagString.append(*fitr);
		flagString.append("  ");
		fitr++;
	}

	QString methodName = method->getName();
	if (QString::compare("<init>", methodName) == 0) {
		methodName = classFile->getThisClass().replace("/", ".");
	}

	QString signature = method->getSignature();
	QString descript = method->getDescriptor();
	QString typeParamter;
	QString methodParamter;
	QString methodthrows;
	QString methodReturn;
	//signature中的是泛型签名，而descript中的是泛型擦除后的签名
	if (!signature.isEmpty()) {
		qDebug() << "method signature :" << signature;
		auto mVisitor = std::make_shared<MethodSignatureVisitor>();
		if (!signature.isEmpty()) {
			SignatureReader sReader(signature);
			sReader.accept(mVisitor);
			typeParamter = mVisitor->getTypeParameters();
			methodReturn = mVisitor->getRetureSignature() + "  ";
			methodParamter = mVisitor->getParamSignature();
			methodthrows = mVisitor->getThrowsSignature();   //throw
		}
	}
	else {
		//若是没有泛型签名,就用descript来进行参数和返回值解析，且不包括异常签名
		int rightIndex = descript.indexOf(")");
		
		QString right = descript.mid(rightIndex+1, descript.size() - rightIndex-1);
		QString param = descript.mid(1, rightIndex-1);

		bool isArray = false;
		if (right.startsWith("[")) {
			isArray = true;
			right = right.mid(1, right.size() - 1);
		}
		if (right.startsWith("L")) {
			methodReturn = right.mid(1, right.size() - 2).replace("/", ".");
		}
		else {
			methodReturn = Util::baseTypeName(right.at(0));
		}

		if (isArray) {
			isArray = false;
			methodReturn += "[]";
		}

		if (!param.isEmpty()) {
			int32_t offset = 0;
			bool first = true;
			while (offset < param.size()) {
				if (!first) {
					methodParamter.append(", ");
				}
				else {
					first = false;
				}
				if (param.at(offset) == '[') {
					isArray = true;
					offset++;
				}
				QString current;
				if (param.at(offset) == 'L') {
					int32_t index = param.indexOf(QChar(';'), offset);
					current = param.mid(offset+1, index - offset-1).replace("/", ".");
					offset = index + 1;
				}
				else {
					current = Util::baseTypeName(param.at(offset));
					offset++;
				}

				if (isArray) {
					isArray = false;
					current.append("[]");
				}

				methodParamter.append(current);
			}
		}
	}
	
	if (!typeParamter.isEmpty()) {
		buffer.append(typeParamter + "  ");
	}
	buffer.append(methodReturn + " ");
	buffer.append(methodName);
	buffer.append("( ");
	buffer.append(methodParamter);
	buffer.append(" )");
	
	if (!methodthrows.isEmpty()) {
		buffer.append("  throws  ");
		buffer.append(methodthrows);
	}
	else{
		auto vec = getMethodExceptions(method->getAttributesRef(), cpool.get());
		if (vec.size() > 0) {
			buffer.append("  throws  ");
			bool first = true;
			for (auto& exception : vec) {
				if (first) {
					first = false;
				}
				else {
					buffer.append(", ");
				}
				buffer.append(exception);
			}
		}
	}
	buffer.append(";");

	items.push_back(createItemNode(buffer));
	items.push_back(createItemNode(QString::fromUtf8(u8"     descriptor: %1").arg(descript)));
	items.push_back(createItemNode(QString::fromUtf8(u8"     flags: (0x%1)  %2").arg(method->getAccessFlags(), 4, 16, QChar('0')).arg(flagString)));

	printCode(method);

	items.push_back(createItemNode(""));
	auto& attributes = method->getAttributesRef();
	for (auto attribute : attributes) {
		if (QString::compare("Signature", attribute->getAttributeName()) == 0) {
			QString buffer = QString::fromLocal8Bit("     Signature: #%1      //%2").arg(method->getSignatureIndex()).arg(signature);
			items.push_back(createItemNode(buffer));
		}
		else {
			printAttributeInfo(attribute.get());
		}
	}

}

void ClassFileModel::printAttributeInfo(AttributeInfo* attr)
{
	auto cpool = classFile->getConstantPool();
	auto attrName = attr->getAttributeName();
	if (QString::compare(attrName, "ConstantValue")==0) {
		uint16_t index = Util::readUnsignedInt16(attr->getBody(), 0);
		auto constant = classFile->getConstantPool()->at(index);
		QString constantTypeName = ConstantTag::translate(constant->getTag());
		QString buffer = QString::fromLocal8Bit("     ConstantValue: #%1            //%2   %3").arg(index).arg(constantTypeName).arg(constant->toString());
		items.push_back(createItemNode(buffer));
	}
	else if (QString::compare(attrName, "Synthetic") == 0 ||
		QString::compare(attrName, "SourceDebugExtension") == 0) {
		items.push_back(createItemNode(QString("     ") + attrName));
	}
	else if (QString::compare(attrName, "RuntimeVisibleAnnotations") == 0 ||
		QString::compare(attrName, "RuntimeInvisibleAnnotations") == 0) {
		printAnnotationAttribute(attr);
	}
	else if (QString::compare(attrName, "RuntimeVisibleParameterAnnotations") == 0 ||
		QString::compare(attrName, "RuntimeInvisibleParameterAnnotations") == 0) {
		printParamterAnnotationAttribute(attr);
	}
	else if (QString::compare(attrName, "RuntimeVisibleTypeAnnotations") == 0 ||
		QString::compare(attrName, "RuntimeInvisibleTypeAnnotations") == 0) {
		printTypeAnnotationAttribute(attr);
	}
	else if (QString::compare(attrName, "Deprecated") == 0) {
		items.push_back(createItemNode("     Deprecated"));
	}
	else if (QString::compare(attrName, "AnnotationDefault") == 0) {
		printAnnotationDefault(attr);
	}
	else if (QString::compare(attrName, "SourceFile") == 0) {
		QString buffer = QString::fromLocal8Bit("     SourceFile: #%1            //%2").arg(classFile->getSourceFileIndex()).arg(classFile->getSourceFile());
		items.push_back(createItemNode(buffer));
	}
	else if (QString::compare(attrName, "InnerClasses") == 0) {
		printInnerClass(attr);
	}
	else if (QString::compare(attrName, "EnclosingMethod") == 0) {
		printEnclosingMethod(attr);
	}
	else if (QString::compare(attrName, "Exceptions")==0) {
		printExceptions(attr);
	}
	else if (QString::compare(attrName, "LineNumberTable")==0) {
		printLineNumberTable(attr);
	}
	else if (QString::compare(attrName, "LocalVariableTable") == 0) {
		printLocalVariableTable(attr);
	}
	else if (QString::compare(attrName, "LocalVariableTypeTable") == 0) {
		printLocalVariableTypeTable(attr);
	}
	else if (QString::compare(attrName, "StackMapTable") == 0) {
		printStackMapTable(attr);
	}
	else if (QString::compare(attrName, "BootstrapMethods") == 0) {
		printBootstrapMethod(attr);
	}
	
}

void ClassFileModel::printAnnotationAttribute(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	uint16_t numAnnotations = Util::readUnsignedInt16(b, 0);
	QString buffer = QString::fromLocal8Bit("     %1, length: %2").arg(attr->getAttributeName()).arg(numAnnotations);
	items.push_back(createItemNode(buffer));
	uint32_t nextOffset = 2;
	auto cpool = classFile->getConstantPool();
	for (uint16_t i = 0; i < numAnnotations; i++) {
		auto cpool = classFile->getConstantPool();
		uint16_t typeIndex = Util::readUnsignedInt16(b, nextOffset);
		QString typeName = cpool->getUtf8(typeIndex);
		uint16_t numElementValuePairs = Util::readUnsignedInt16(b, nextOffset+2);
		QString elementValuePairsbuffer;
		elementValuePairsbuffer.append(QString::fromLocal8Bit("          %1: #%2").arg(QString::number(i))
																				   .arg(QString::number(typeIndex)));
		nextOffset += 4;
		if (numElementValuePairs == 0) {
			elementValuePairsbuffer.append("()         //").append(Util::javaClassName(typeName));
			items.push_back(createItemNode(elementValuePairsbuffer));
			continue;
		}
		else {
			elementValuePairsbuffer.append("( ");
			QString valuePairsBuffer(Util::javaClassName(typeName)+"(");
			for (uint16_t j = 0; j < numElementValuePairs; j++) {
				uint32_t annoLength = printElementValuePair(b + nextOffset, elementValuePairsbuffer, valuePairsBuffer);
				nextOffset += annoLength;
				if (j != numElementValuePairs - 1) {
					elementValuePairsbuffer.append(", ");
					valuePairsBuffer.append(",");
				}
			}
			elementValuePairsbuffer.append(" )");
			valuePairsBuffer.append(" )");
			elementValuePairsbuffer.append("          //").append(valuePairsBuffer);
			items.push_back(createItemNode(elementValuePairsbuffer));
		
		}
		
	}
}

uint32_t ClassFileModel::printElementValuePair(const char* b, QString& buffer, QString& valueBuffer)
{
	auto cpool = classFile->getConstantPool();
	uint16_t elementNameIndex = Util::readUnsignedInt16(b, 0);
	buffer.append(QString::fromLocal8Bit("#%1=").arg(elementNameIndex));
	QString elementName = cpool->getUtf8(elementNameIndex);
	valueBuffer.append(elementName);
	valueBuffer.append("=");

	uint32_t valueLength = printElementValue(b + 2, buffer, valueBuffer);
	return valueLength + 2;
}

uint32_t ClassFileModel::printElementValue(const char* b, QString& buffer, QString& valueBuffer)
{
	auto cpool = classFile->getConstantPool();
	char tag = *b;
	uint32_t nextOffset = 1;
	switch (tag) {
	case 'B':
	case 'C':
	case 'I':
	case 'S':
	case 'Z':
	case 'D':
	case 'F':
	case 'J':
	case 'c':
	case 's':
	{
		uint16_t constValueIndex = Util::readUnsignedInt16(b, nextOffset);
		buffer.append(QString::fromLocal8Bit("%1#%2").arg(QChar(tag)).arg(QString::number(constValueIndex)));
		nextOffset += 2;
		auto constValueInfo = cpool->at(constValueIndex);
		QString constValue = constValueInfo->toString();
		if (tag == 's') {
			constValue = "\"" + constValue + "\"";
		}
		valueBuffer.append(constValue);
	}
	break;
	case 'e':
	{
		uint16_t typeNameIndex = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		QString typeName = cpool->getUtf8(typeNameIndex);
		uint16_t constNameIndex = Util::readUnsignedInt16(b, nextOffset);
		buffer.append(QString::fromLocal8Bit("e#%1.#%2").arg(QString::number(typeNameIndex)).arg(QString::number(constNameIndex)));
		nextOffset += 2;
		QString constName = cpool->getUtf8(constNameIndex);
		valueBuffer.append(QString::fromLocal8Bit("%1.%2").arg(typeName).arg(constName));
	}
	break;
	case '@':
	{
		uint16_t typeIndex = Util::readUnsignedInt16(b, nextOffset);
		QString typeName = cpool->getUtf8(typeIndex);
		buffer.append(QString::fromLocal8Bit("@#%1(").arg(QString::number(typeIndex)));
		valueBuffer.append(QString::fromLocal8Bit("@%1(").arg(Util::javaClassName(typeName)));

		uint16_t numElementValuePairs = Util::readUnsignedInt16(b, nextOffset + 2);
		nextOffset += 4;
		if (numElementValuePairs == 0) {
			buffer.append(")");
			valueBuffer.append(")");
		}
		else {
			for (uint32_t i = 0; i < numElementValuePairs; i++) {
				uint32_t length = printElementValuePair(b + nextOffset, buffer, valueBuffer);
				nextOffset += length;
				if (i != numElementValuePairs - 1) {
					buffer.append(" , ");
					valueBuffer.append(" , ");
				}
			}
			buffer.append(")");
			valueBuffer.append(")");
		}
	}
	break;
	case '[':
	{
		uint16_t numValues = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		buffer.append("[");
		valueBuffer.append("[");
		for(int i =0; i< numValues; i++){
			uint32_t length = printElementValue(b + nextOffset, buffer, valueBuffer);
			nextOffset += length;
			if (i < numValues-1) {
				buffer.append(" , ");
				valueBuffer.append(" , ");
			}
		}
		buffer.append(" ]");
		valueBuffer.append(" ]");
	}
	break;
	default:
		break;
	}
	return nextOffset;
}

void ClassFileModel::printTypeAnnotationAttribute(AttributeInfo* attr)
{
	const uint32_t length = attr->getBodyLength();
	const char* b = attr->getBody();
	uint16_t numTypeAnnotations = Util::readUnsignedInt16(b, 0);
	QString buffer = QString::fromLocal8Bit("     %1:").arg(attr->getAttributeName());
	items.push_back(createItemNode(buffer));
	if (numTypeAnnotations == 0) {
		return;
	}
	uint32_t nextOffset = 2;
	auto cpool = classFile->getConstantPool();
	for (uint32_t i = 0; i < numTypeAnnotations; i++) {
		/*QString typePathBuffer;
		uint32_t length = printTypeAnnotationTarget(b + nextOffset, typePathBuffer);
		nextOffset += length;

		uint32_t pathLength = printTypeAnnotationPath(b + nextOffset, typePathBuffer);
		nextOffset += pathLength;*/

		uint32_t len;
		auto pos = Position::read_position(b + nextOffset, len);
		nextOffset += len;

		QString typeAnnotationBuffer;
		uint16_t typeIndex = Util::readUnsignedInt16(b, nextOffset);
		QString typeName = cpool->getUtf8(typeIndex);
		uint16_t numElementValuePairs = Util::readUnsignedInt16(b, nextOffset + 2);
		typeAnnotationBuffer.append(QString::fromLocal8Bit("          %1: #%2").arg(QString::number(i))
			                                                                   .arg(QString::number(typeIndex)));
		nextOffset += 4;
		if (numElementValuePairs == 0) {
			typeAnnotationBuffer.append("(): ").append(pos->toString());
			typeAnnotationBuffer.append("          //").append(Util::javaClassName(typeName));
			items.push_back(createItemNode(typeAnnotationBuffer));
			continue;
		}
		typeAnnotationBuffer.append("( ");
		QString annotationValueBuffer(Util::javaClassName(typeName) + "(");
		for (uint16_t j = 0; j < numElementValuePairs; j++) {
			uint32_t annoLength = printElementValuePair(b + nextOffset, typeAnnotationBuffer, annotationValueBuffer);
			nextOffset += annoLength;
			if (j != numElementValuePairs - 1) {
				typeAnnotationBuffer.append(", ");
				annotationValueBuffer.append(", ");
			}
		}
		typeAnnotationBuffer.append(" ): ");
		annotationValueBuffer.append(" )");
		typeAnnotationBuffer.append(pos->toString());
		typeAnnotationBuffer.append("            //").append(annotationValueBuffer);
		items.push_back(createItemNode(typeAnnotationBuffer));
	}
}

uint32_t ClassFileModel::printTypeAnnotationTarget(const char* b, QString& buffer)
{
	//jvmtiRedefineClasses.cpp   skip_type_annotation_target方法
	char type = *b;
	uint32_t nextOffset = 1;
	switch (type) {
		case 0x00:
		case 0x01:
		{
			// struct:
			// type_parameter_target {
			//   u1 type_parameter_index;   //添加到哪个类型参数注解上
			// }
			//
			uint8_t  typeParameterIndex = *(b + nextOffset);
			nextOffset += 1;
		} break;
		case 0x10:
		{
			// struct:
			// supertype_target {
			//   u2 supertype_index;  //若是值是65535表示是父类，其他值是接口在常量池中的索引
			// }
			uint16_t superTypeIndex = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
		}break;
		case 0x11:
		case 0x12:
		{
			// struct:
			// type_parameter_bound_target {
			//   u1 type_parameter_index;   //是哪个类型参数，参考type_parameter_target
			//   u1 bound_index;   //该类型参数的哪个界限,并没有记录界限处的类型信息
			// }
			uint8_t  typeParameterIndex = *(b + nextOffset);
			nextOffset += 1;
			uint8_t  boundIndex = *(b + nextOffset);
			nextOffset += 1;
		}break;
		case 0x13:  // location: field_info // kind: type in field declaration
		case 0x14:  // location: method_info
		case 0x15:  // location: method_info
		{
			// empty_target {
			// }
		}break; 
		case 0x16:   // kind: type in formal parameter declaration of method, constructor, or lambda expression
		{
			 // struct:
		     // formal_parameter_target {
		     //   u1 formal_parameter_index;
		     // }
			uint8_t  formalParameterIndex = *(b + nextOffset);
			nextOffset += 1;
		}break;
		case 0x17:  // kind: type in throws clause of method or constructor
		{
			// struct:
			// throws_target {
			//   u2 throws_type_index    //exceptiontable中的index值, 不是常量池中的索引
			// }
			uint16_t throwTypeIndex = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
		}break;
		case 0x40:   // kind: type in local variable declaration, in code
		case 0x41:   // kind: type in resource variable declaration
		{
			 // struct:
			 // localvar_target {
			 //   u2 table_length;
			 //   struct {
			 //     u2 start_pc;    //在Code中的偏移量
			 //     u2 length;    //在这个范围内是有值的
			 //     u2 index;    //当前帧中的索引
			 //   } table[table_length];
			 // }
			uint16_t tableLength = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			int tableStructSize = 2 + 2 + 2;
			if (tableLength > 0) {
				for (uint32_t i = 0; i < tableLength; i++) {
					uint16_t startPc = Util::readUnsignedInt16(b, nextOffset);  
					uint16_t length = Util::readUnsignedInt16(b, nextOffset + 2);
					uint16_t index = Util::readUnsignedInt16(b, nextOffset + 4);   
					nextOffset += tableStructSize;
				}
			}
		}break;
		case 0x42:  // kind: type in exception parameter declaration. location code
		{
			// struct:
			// catch_target {
			//   u2 exception_table_index;
			// }

			uint16_t exceptionTableIndex = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;

		} break;
		case 0x43:  // kind: type in instanceof expression,location: Code
		case 0x44:  // kind: type in new expression. location: Code
		case 0x45:  // kind: type in method reference expression using ::new.  location: Code
		case 0x46:  // kind: type in method reference expression using ::Identifier. location: Code
		{
			// struct:
			// offset_target {
			//   u2 offset;
			// }
			//

			uint16_t offset = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;

		} break;
		case 0x47:   // kind: type in cast expression. location: Code
		case 0x48:   // kind: type argument for generic constructor in new expression or.explicit constructor invocation statement.location: Code
		case 0x49:   // kind: type argument for generic method in method invocation expression
		case 0x4A:   // kind: type argument for generic constructor in method reference expression using ::new.  location: Code
		case 0x4B:   // kind: type argument for generic method in method reference expression using ::Identifier. location: Code
		{
			// struct:
			// type_argument_target {
			//   u2 offset;
			//   u1 type_argument_index;
			// }
			//

			uint16_t offset = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			uint8_t type_argument_index = *(b + nextOffset);
			nextOffset += 1;
		} break;
		default: {
		}
	}
	return nextOffset;
}

uint32_t ClassFileModel::printTypeAnnotationPath(const char* b, QString& buffer)
{
	//skip_type_annotation_type_path
	uint8_t pathLength = *b;
	uint32_t nextOffset = 1;
	for (uint8_t i = 0; i < pathLength; i++) {
		uint8_t typePathKind = *(b + nextOffset);
		uint8_t typeArgumentIndex = *(b + nextOffset + 1);
		nextOffset += 2;
	}

	return 2*pathLength + 1;
}

void ClassFileModel::printAnnotationDefault(AttributeInfo* attr)
{
	QString buffer = QString::fromLocal8Bit("     AnnotationDefault:  ");
	const char* b = attr->getBody();
	QString valueBuffer;
	printElementValue(b, buffer, valueBuffer);
	buffer.append("           //").append(valueBuffer);
	items.push_back(createItemNode(buffer));
}

void ClassFileModel::printInnerClass(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	uint16_t numberOfClasses = Util::readUnsignedInt16(b, 0);
	items.push_back(createItemNode(QString::fromLocal8Bit("     InnerClasses. size:%1").arg(numberOfClasses)));
	auto cpool = classFile->getConstantPool();
	uint32_t nextOffset = 2;
	for (uint16_t i = 0; i < numberOfClasses; i++) {
		uint16_t innerClassInfoIndex = Util::readUnsignedInt16(b, nextOffset);
		QString innerClass = cpool->getClassName(innerClassInfoIndex);
		uint16_t outerClassInfoIndex = Util::readUnsignedInt16(b, nextOffset+2);
		QString outerClass;
		if (outerClassInfoIndex != 0) {
			outerClass = cpool->getClassName(outerClassInfoIndex);
		}
		uint16_t innerNameIndex = Util::readUnsignedInt16(b, nextOffset+4);
		QString  innerName;
		if (innerNameIndex != 0) {
			innerName = cpool->getUtf8(innerNameIndex);
		}
		uint16_t innerClassAccessFlags = Util::readUnsignedInt16(b, nextOffset+6);

		QString innerClassInfo = QString::fromLocal8Bit("          InnerClassInfo: %1 (#%2), OutClassInfo: %3 (#%4), InnerName: %5 (#%6)")
														.arg(innerClass).arg(innerClassInfoIndex)
														.arg(outerClass).arg(outerClassInfoIndex)
														.arg(innerName).arg(innerNameIndex);
		items.push_back(createItemNode(innerClassInfo));
		nextOffset += 8;
	}
}

void ClassFileModel::printEnclosingMethod(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	auto cpool = classFile->getConstantPool();
	uint16_t classIndex = Util::readUnsignedInt16(b, 0);
	QString className = cpool->getClassName(classIndex);
	uint16_t methodIndex = Util::readUnsignedInt16(b, 2);
	QString methodName;
	if (methodIndex) {
		methodName = cpool->getNameAndType(methodIndex);
	}
	QString buffer = QString::fromLocal8Bit("     EnclosingMethod:  #%1.#%2       // %3.%4").arg(classIndex)
		.arg(methodIndex).arg(className).arg(methodName);
	items.push_back(createItemNode(buffer));
}

void ClassFileModel::printBootstrapMethod(AttributeInfo* attr)
{
	auto cpool = classFile->getConstantPool();
	const char* b = attr->getBody();
	uint16_t numBootstrapMethods = Util::readUnsignedInt16(b, 0);
	uint32_t nextOffset = 2;
	items.push_back(createItemNode("     BootstrapMethods:"));
	for (uint16_t i = 0; i < numBootstrapMethods; i++) {
		uint16_t bootstrapMethodRef = Util::readUnsignedInt16(b, nextOffset);
		QString bootstrapMethod = QString::fromLocal8Bit("          %1: #%2  %3").arg(i)
			                                           .arg(bootstrapMethodRef)
			                                           .arg(cpool->getGeneral(bootstrapMethodRef));
		items.push_back(createItemNode(bootstrapMethod));
		items.push_back(createItemNode("               Method arguments:"));
		uint16_t numBootstrapMethodArguments = Util::readUnsignedInt16(b, nextOffset+2);
		nextOffset += 4;
		for (uint16_t j = 0; j < numBootstrapMethodArguments; j++) {
			uint16_t bootstrapArgumentIndex = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			items.push_back(createItemNode(QString::fromLocal8Bit("                    #%1  %2").arg(bootstrapArgumentIndex).arg(cpool->getGeneral(bootstrapArgumentIndex))));
		}
		
	}
}

void ClassFileModel::printExceptions(AttributeInfo* attr)
{
	auto cpool = classFile->getConstantPool();
	const char* b = attr->getBody();
	uint16_t numberOfExceptions = Util::readUnsignedInt16(b, 0);
	QString exceptionBuffer = QString::fromLocal8Bit("     Exceptions, number: %1 .").arg(numberOfExceptions);
	if (numberOfExceptions > 0) {
		uint32_t nextOffset = 2;
		exceptionBuffer.append(" Type:[ ");
		for(uint16_t i = 0; i < numberOfExceptions; i++) {
			uint16_t exceptionIndex = Util::readUnsignedInt16(b, nextOffset);
			QString excetionClass = cpool->getClassName(exceptionIndex);
			exceptionBuffer.append(excetionClass);
			if (i != numberOfExceptions - 1) {
				exceptionBuffer.append(" , ");
			}
			nextOffset += 2;
		}
		exceptionBuffer.append(" ]");
	}
	items.push_back(createItemNode(exceptionBuffer));
}

void ClassFileModel::printLineNumberTable(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	auto cpool = classFile->getConstantPool();
	uint16_t lineNumberTableLength = Util::readUnsignedInt16(b, 0);
	QString tableHeaderBuffer = QString::fromLocal8Bit("         LineNumberTable, length: %1 .").arg(lineNumberTableLength);
	items.push_back(createItemNode(tableHeaderBuffer));
	uint32_t nextOffset = 2;
	for (uint16_t i = 0; i < lineNumberTableLength; i++) {
		uint16_t startPc = Util::readUnsignedInt16(b, nextOffset);
		uint16_t lineNumber = Util::readUnsignedInt16(b, nextOffset+2);
		items.push_back(createItemNode(QString::fromLocal8Bit("                line %1: %2").arg(lineNumber).arg(startPc)));
		nextOffset += 4;
	}
}

void ClassFileModel::printLocalVariableTable(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	auto cpool = classFile->getConstantPool();
	uint16_t tableLength = Util::readUnsignedInt16(b, 0);
	QString tableHeaderBuffer = QString::fromLocal8Bit("         LocalVariableTable, length: %1 .").arg(tableLength);
	items.push_back(createItemNode(tableHeaderBuffer));
	uint32_t nextOffset = 2;
	items.push_back(createItemNode("                Start   Length   Slot   Name     Signature"));
	for (uint16_t i = 0; i < tableLength; i++) {
		uint16_t startPc = Util::readUnsignedInt16(b, nextOffset);
		uint16_t length = Util::readUnsignedInt16(b, nextOffset + 2);
		uint16_t nameIndex = Util::readUnsignedInt16(b, nextOffset + 4);
		uint16_t descIndex = Util::readUnsignedInt16(b, nextOffset + 6);
		uint16_t index = Util::readUnsignedInt16(b, nextOffset + 8);
		QString item = QString::fromLocal8Bit("                %1     %2     %3       %4      %5")
			.arg(startPc, 4, 10, QChar(' '))
			.arg(length, 4, 10, QChar(' '))
			.arg(index, 4, 10, QChar(' '))
			.arg(cpool->getUtf8(nameIndex), 5, QChar(' '))
			.arg(cpool->getUtf8(descIndex));
		items.push_back(createItemNode(item));
		nextOffset += 10;
	}
}

void ClassFileModel::printLocalVariableTypeTable(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	auto cpool = classFile->getConstantPool();
	uint16_t tableLength = Util::readUnsignedInt16(b, 0);
	QString tableHeaderBuffer = QString::fromLocal8Bit("         LocalVariableTypeTable, length: %1 .").arg(tableLength);
	items.push_back(createItemNode(tableHeaderBuffer));
	uint32_t nextOffset = 2;
	items.push_back(createItemNode("                Start   Length   Slot   Name     Signature"));
	for (uint16_t i = 0; i < tableLength; i++) {
		uint16_t startPc = Util::readUnsignedInt16(b, nextOffset);
		uint16_t length = Util::readUnsignedInt16(b, nextOffset + 2);
		uint16_t nameIndex = Util::readUnsignedInt16(b, nextOffset + 4);
		uint16_t signatureIndex = Util::readUnsignedInt16(b, nextOffset + 6);
		uint16_t index = Util::readUnsignedInt16(b, nextOffset + 8);
		QString item = QString::fromLocal8Bit("                %1     %2     %3       %4      %5")
			.arg(startPc, 4, 10, QChar(' '))
			.arg(length, 4, 10, QChar(' '))
			.arg(index, 4, 10, QChar(' '))
			.arg(cpool->getUtf8(nameIndex), 5, QChar(' '))
			.arg(cpool->getUtf8(signatureIndex));
		items.push_back(createItemNode(item));
		nextOffset += 10;
	}
}

//openjdk11.0.6.5/src/hotspot/share/prims/jvmtiRedefineClasses.cpp
void ClassFileModel::printStackMapTable(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	uint16_t numberOfEntries = Util::readUnsignedInt16(b, 0);
	QString stackMapBuffer = QString::fromLocal8Bit("          StackMapTable: number_of_entries = %1").arg(numberOfEntries);
	items.push_back(createItemNode(stackMapBuffer));
	uint32_t nextOffset = 2;
	uint32_t preOffset;
	for (uint16_t i = 0; i < numberOfEntries; i++) {
		 // union stack_map_frame {
		 //   same_frame;
		 //   same_locals_1_stack_item_frame;
		 //   same_locals_1_stack_item_frame_extended;
		 //   chop_frame;
		 //   same_frame_extended;
		 //   append_frame;
		 //   full_frame;
		 // }
		uint8_t frame_type = *(b + nextOffset);
		int offset;
		nextOffset++;
		if (frame_type < 64) {
			// same_frame
			if (i==0) {
				offset = frame_type;
				preOffset = offset;
			}
			else {
				offset = preOffset + frame_type + 1;
			}
			preOffset = offset;
			QString sameBuffer = QString::fromLocal8Bit("                frame_type = %1  /* same */").arg(frame_type);
			items.push_back(createItemNode(sameBuffer));
		}
		else if (frame_type < 128) {
		
		}
		else if (frame_type < 247) {
			//todo
		}
		else if (frame_type == 247) {
		
		}
		else if (frame_type >= 248 && frame_type <= 250) {
			uint16_t offsetDelta = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			QString chopBuffer = QString::fromLocal8Bit("                frame_type = %1  /* chop */ , offset_delta = %2 ").arg(frame_type).arg(offsetDelta);
			items.push_back(createItemNode(chopBuffer));
		}
		else if (frame_type == 251) {
			uint16_t offsetDelta = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			QString sameExtendedBuffer = QString::fromLocal8Bit("                frame_type = %1  /* same_frame_extended */ , offset_delta = %2 ").arg(frame_type).arg(offsetDelta);
			items.push_back(createItemNode(sameExtendedBuffer));
		}
		else if (frame_type >= 252 && frame_type <= 254) {
			uint16_t offsetDelta = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			QString appendBuffer = QString::fromLocal8Bit("                frame_type = %1  /* append */ , offset_delta = %2, locals = [").arg(frame_type).arg(offsetDelta);
			uint16_t appendCount = frame_type - 251;
			for (uint16_t j = 0; j < appendCount; j++) {
				uint32_t length = verificationTypeInfo(b + nextOffset, j, frame_type, appendBuffer);
				nextOffset += length;
				if (j != appendCount - 1) {
					appendBuffer.append(" , ");
				}
			}
			appendBuffer.append(" ]");

			items.push_back(createItemNode(appendBuffer));

		}
		else if (frame_type == 255) {
			uint16_t offsetDelta = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			uint16_t numberOfLocals = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			QString fullBuffer = QString::fromLocal8Bit("                frame_type = %1  /* full_frame */ , offset_delta = %2, locals = [").arg(frame_type).arg(offsetDelta);
			for (uint16_t j = 0; j < numberOfLocals; j++) {
				uint32_t length = verificationTypeInfo(b + nextOffset, j, frame_type, fullBuffer);
				nextOffset += length;
				if (j != numberOfLocals - 1) {
					fullBuffer.append(" , ");
				}
			}
			fullBuffer.append(" ], stack=[");
			uint16_t numberOfStackItems = Util::readUnsignedInt16(b, nextOffset);
			nextOffset += 2;
			for (uint16_t j = 0; j < numberOfStackItems; j++) {
				uint32_t length = verificationTypeInfo(b + nextOffset, j, frame_type, fullBuffer);
				nextOffset += length;
				if (j != numberOfLocals - 1) {
					fullBuffer.append(" , ");
				}
			}
			fullBuffer.append(" ]");
			items.push_back(createItemNode(fullBuffer));
		}
	
	}
}

uint32_t ClassFileModel::verificationTypeInfo(const char* b, uint16_t indexOfTable, uint8_t frameType, QString& buffer)
{
	auto cpool = classFile->getConstantPool();
	uint8_t tag = *b;
	uint32_t nextOffset = 1;

	switch (tag) {
		// Top_variable_info {
		//   u1 tag = ITEM_Top; /* 0 */
		// }
		// verificationType.hpp has zero as ITEM_Bogus instead of ITEM_Top
	case 0:  // fall through
	// Integer_variable_info {
	//   u1 tag = ITEM_Integer; /* 1 */
	// }
	case ITEM_Integer:  // fall through
	// Float_variable_info {
	//   u1 tag = ITEM_Float; /* 2 */
	// }
	{
		buffer.append("int");
	}break;
	case ITEM_Float:  // fall through
	{
		buffer.append("float");
	}break;
	// Double_variable_info {
	//   u1 tag = ITEM_Double; /* 3 */
	// }
	case ITEM_Double:  // fall through
	{
		buffer.append("double");
	}break;
	// Long_variable_info {
	//   u1 tag = ITEM_Long; /* 4 */
	// }
	case ITEM_Long:  // fall through
	{
		buffer.append("long");
	}break;
	// Null_variable_info {
	//   u1 tag = ITEM_Null; /* 5 */
	// }
	case ITEM_Null:  // fall through
	{
		buffer.append("null");
	}break;
	// UninitializedThis_variable_info {
	//   u1 tag = ITEM_UninitializedThis; /* 6 */
	// }
	case ITEM_UninitializedThis:
		// nothing more to do for the above tag types
	{
		buffer.append("UninitializedThis");
	}
	break;
	// Object_variable_info {
	//   u1 tag = ITEM_Object; /* 7 */
	//   u2 cpool_index;
	// }
	case ITEM_Object:
	{
		uint16_t cpoolIndex = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		buffer.append(cpool->getClassName(cpoolIndex));
		/*u2 new_cp_index = find_new_index(cpoolIndex);
		if (new_cp_index != 0) {
			log_debug(redefine, class, stackmap)("mapped old cpool_index=%d", cpool_index);
			Bytes::put_Java_u2(stackmap_p_ref, new_cp_index);
			cpool_index = new_cp_index;
		}
		stackmap_p_ref += 2;*/
	} break;

	// Uninitialized_variable_info {
	//   u1 tag = ITEM_Uninitialized; /* 8 */
	//   u2 offset;
	// }
	case ITEM_Uninitialized:
		/*stackmap_p_ref += 2;*/
		buffer.append("Uninitialized_variable");
		nextOffset += 2;
		break;

	default:
		break;
	} // end switch (tag)

	return nextOffset;
}

void ClassFileModel::printParamterAnnotationAttribute(AttributeInfo* attr)
{
	const char* b = attr->getBody();
	uint8_t numParamters =*b;
	QString buffer = QString::fromLocal8Bit("     %1:").arg(attr->getAttributeName()).arg(numParamters);
	items.push_back(createItemNode(buffer));

	uint32_t nextOffset = 1;
	auto cpool = classFile->getConstantPool();
	for (uint16_t i = 0; i < numParamters; i++) {
		uint16_t numAnnotations = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		QString paramterBuffer = QString::fromLocal8Bit("        parameter %1:").arg(i);
		items.push_back(createItemNode(paramterBuffer));
		for (uint16_t j = 0; j < numAnnotations; j++) {
			uint16_t typeIndex = Util::readUnsignedInt16(b, nextOffset);
			QString typeName = cpool->getUtf8(typeIndex);
			uint16_t numElementValuePairs = Util::readUnsignedInt16(b, nextOffset + 2);
			QString elementValuePairsbuffer;
			elementValuePairsbuffer.append(QString::fromLocal8Bit("           %1: #%2").arg(QString::number(i))
				                                                                               .arg(QString::number(typeIndex)));
			nextOffset += 4;
			if (numElementValuePairs == 0) {
				elementValuePairsbuffer.append("()         //").append(Util::javaClassName(typeName));
				items.push_back(createItemNode(elementValuePairsbuffer));
				continue;
			}
			elementValuePairsbuffer.append("( ");
			QString valuePairsBuffer(Util::javaClassName(typeName) + "(");
			for (uint16_t j = 0; j < numElementValuePairs; j++) {
				uint32_t annoLength = printElementValuePair(b + nextOffset, elementValuePairsbuffer, valuePairsBuffer);
				nextOffset += annoLength;
				if (j != numElementValuePairs - 1) {
					elementValuePairsbuffer.append(", ");
					valuePairsBuffer.append(",");
				}
			}
			elementValuePairsbuffer.append(" )");
			valuePairsBuffer.append(" )");
			elementValuePairsbuffer.append("          //").append(valuePairsBuffer);
			items.push_back(createItemNode(elementValuePairsbuffer));
		}
	}
}

void ClassFileModel::printCode(MethodInfo* method)
{
	auto code = method->getCodeAttribute();
	if (code == nullptr) {
		return;
	}

	items.push_back(createItemNode(QString::fromUtf8(u8"     Code:")));
	uint16_t maxStack = code->getMaxStack();
	uint16_t maxLocals = code->getMaxLocals();

	auto params = method->getParamters();
	uint8_t parameterLength = 0;
	if (params.get() != nullptr) {
		parameterLength = *params->getBody();
	}
	items.push_back(createItemNode( QString::fromUtf8(u8"         stack=%1, locals=%2, args_size=%3").arg(maxStack).arg(maxLocals).arg(parameterLength)));

	qDebug() << "method name:" << method->getName();
	code->each(*this);

	auto& attributes = code->getAttributes();
	for (auto attribute : attributes) {
		printAttributeInfo(attribute.get());
	}
}

void ClassFileModel::printClassAttributeInfo()
{
	items.push_back(createItemNode("Attributes:"));
	auto& attrs = classFile->getAttributes();
	for (auto attr : attrs) {
		if (QString::compare("Signature", attr->getAttributeName()) == 0) {
			QString buffer = QString::fromLocal8Bit("     Signature: #%1      //%2").arg(classFile->getSignatureIndex()).arg(classFile->getSignature());
			items.push_back(createItemNode(buffer));
			continue;
		}
		printAttributeInfo(attr.get());
	}
}

void ClassFileModel::visit(const InstructionContext& context)
{
	QString parameteFormatter = context.formatter;
	QString fullString;
	QString codeName = context.codeName;
	if (context.wide) {
		codeName = codeName + "_w";
	}
	auto cpool = classFile->getConstantPool();
	if (parameteFormatter.isEmpty() || parameteFormatter.size() == 0) {
		if (context.opcode == Bytecodes::_lookupswitch) {
			fullString.append(codeName).append("   {");
			map<const QString, int32_t>* s = static_cast<map<const QString, int32_t>*>(context.extra);
			for (auto& itr : *s) {
				fullString.append(QString::fromLocal8Bit(" %1:%2 ").arg(itr.first).arg(itr.second));
			}
			fullString.append(" } ");
		}
		else if (context.opcode == Bytecodes::_tableswitch) {
			fullString.append(codeName).append("   {");
			vector<int32_t>* v = static_cast<vector<int32_t>*>(context.extra);
			int32_t size = v->size();
			for (int32_t i = 0; i < size - 1; i++) {
				fullString.append(QString::fromLocal8Bit(" %1:%2 ").arg(i).arg(v->at(i)));
			}
			fullString.append(QString::fromLocal8Bit(" %1:%2 ").arg("default").arg(v->at(size - 1)));
			fullString.append(" } ");
		}
		else{
			fullString.append(context.codeName + "  " + context.remark);
		}
	}
	else {
		if (QString::compare("c", parameteFormatter) == 0 ||
			QString::compare("cc", parameteFormatter) == 0 ||
			QString::compare("i", parameteFormatter) == 0 ||
			QString::compare("ii", parameteFormatter) == 0) {
			fullString.append(codeName).append("   ").append(QString::number(context.parameter1));
		}
		else if (QString::compare("oo", parameteFormatter) == 0 ||
			QString::compare("oooo", parameteFormatter) == 0) {
			uint32_t o1 = context.offset + context.parameter1;
			fullString.append(codeName).append(" ").append(QString::number(o1));
		}
		else if (QString::compare("JJ", parameteFormatter) == 0 
			|| QString::compare("k", parameteFormatter) == 0
			|| QString::compare("kk", parameteFormatter) == 0) {
			uint16_t index = context.parameter1;
			auto info = cpool->at(index);
			auto type = ConstantTag::translate(info->getTag(), false);
			fullString.append(codeName).append(" #").append(QString::number(index)).append("          //").append(type).append("   ").append(info->toString());
		}
		else if (QString::compare("JJJJ", parameteFormatter) == 0) {
			uint16_t index = context.parameter1;
			auto dynamicInfoText = cpool->getGeneral(index);
			fullString.append(codeName).append("  #").append(QString::number(index)).append(", 0          // InvokeDynamic  ").append(dynamicInfoText);
		}
		else if (QString::compare("JJ__", parameteFormatter) == 0) {
			uint16_t index = context.parameter1;
			auto interfaceText = cpool->getGeneral(context.parameter1);
			fullString.append(codeName).append("  #").append(QString::number(index)).append(",  2         // InterfaceMethod ").append(interfaceText);
		}
		else if (context.opcode == Bytecodes::_iinc) {
			fullString.append(codeName).append(QString::fromLocal8Bit("        %1  %2")
				                                          .arg(QString::number(context.parameter1))
				                                          .arg(QString::number(context.parameter2)));
		}
		else if (context.opcode == Bytecodes::_multianewarray) {
			uint16_t index = context.parameter1;
			auto info = cpool->at(index);
			auto type = ConstantTag::translate(info->getTag(), false);
			fullString.append(codeName).append(QString::fromLocal8Bit("  #%1  %2").arg(QString::number(index)).arg(QString::number(context.parameter2)))
				                       .append("          //").append(type).append("  ").append(info->toString());

		}
		else {
			fullString.append(codeName).append(" "+context.remark);
		}
	}
	items.push_back(createItemNode( QString::fromUtf8(u8"             %1:  %2").arg(context.offset, 5, 10, QChar(' ') ).arg(fullString) ));
}

std::vector<QString> ClassFileModel::getMethodExceptions(const std::vector<std::shared_ptr<AttributeInfo>>& attrs, const ConstantPool* cpool)
{
	for (auto& attr : attrs) {
		if (QString::compare(attr->getAttributeName(), "Exceptions") == 0) {
			uint16_t exceptionCount = Util::readUnsignedInt16(attr->getBody(), 0);
			if (exceptionCount > 0) {
				std::vector<QString> vec;
				for (uint16_t i = 0; i < exceptionCount; i++) {
					uint16_t exceptionIndex = Util::readUnsignedInt16(attr->getBody(), 2 * i + 2);
					vec.push_back(cpool->getClassName(exceptionIndex).replace("/", "."));
				}
				return std::move(vec);
			}
		}
	}
	return std::vector<QString>();
}


int ClassFileModel::rowCount(const QModelIndex& parent) const
{
	return items.size();
}

QVariant ClassFileModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if (index.row() >= items.size()) {
		return QVariant();
	}
	if (role == Qt::DisplayRole) {
		return items[index.row()]->text;
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

QModelIndex ClassFileModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid() && row<items.size()) {
		return createIndex(row, column, items[row].get());
	}
	return QModelIndex();
}


