#pragma once

#include<memory>
#include "classFile.h"
#include "signature.h"
#include "viewcommon.h"

using namespace jdk;

using std::weak_ptr;
using std::shared_ptr;

class ClassFileModel :
	public QAbstractListModel,
	public CodeVisitor
{
public:
	ClassFileModel(const ClassFile* cf, QObject* parent=nullptr);

	void resetNodeType(NodeType, void*);

	QString md5(QString fileName) const;
	QString modified(QString fileName) const;
	uint32_t fileSize(QString fileName) const;
	QString getSourceFile() const;
	QString getClassFullName() const;
	QString getClassFlagsName() const;

	shared_ptr<ListItem> createItemNode(QString text);

	void printClassDigest();
	void printVersion();
	void printConstantPool();
	void printAccessFlags();
	void printThisClass();
	void printSuperClass();
	void printInterfaces();

	void printFields();
	void printField(FieldInfo* field);

	void printMethods();
	void printMethod(MethodInfo* method);
	void printCode(MethodInfo* method);

	void printExceptions(AttributeInfo*);
	void printLineNumberTable(AttributeInfo*);
	void printLocalVariableTable(AttributeInfo*);
	void printLocalVariableTypeTable(AttributeInfo*);
	void printStackMapTable(AttributeInfo*);
	uint32_t verificationTypeInfo(const char*, uint16_t indexOfTable, uint8_t frameType, QString& buffer);

	void printClassAttributeInfo();

	void printAttributeInfo(AttributeInfo*);

	void printAnnotationAttribute(AttributeInfo*);
	uint32_t printElementValuePair(const char*, QString& buffer, QString& valueBuffer);
	uint32_t printElementValue(const char*, QString& buffer, QString& valueBuffer);

	void printParamterAnnotationAttribute(AttributeInfo*);

	void printTypeAnnotationAttribute(AttributeInfo*);
	uint32_t printTypeAnnotationTarget(const char*, QString& buffer);
	uint32_t printTypeAnnotationPath(const char*, QString& buffer);

	void printAnnotationDefault(AttributeInfo*);

	void printInnerClass(AttributeInfo*);
	void printEnclosingMethod(AttributeInfo*);

	void printBootstrapMethod(AttributeInfo*);

	void visit(const InstructionContext& context) override;

	std::vector<QString> getMethodExceptions(const std::vector<std::shared_ptr<AttributeInfo>>&, const ConstantPool* cpool);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

private:
	const ClassFile* classFile;
	vector<shared_ptr<ListItem>> items;
};

