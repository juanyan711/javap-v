#pragma once

#include<memory>
#include "classFile.h"
#include "signature.h"

using namespace jdk;

class ClassFileModel :
	public QAbstractListModel
{
public:
	ClassFileModel(std::shared_ptr<ClassFile>& cf, QObject* parent=nullptr);

	QString md5(QString fileName) const;
	QString modified(QString fileName) const;
	uint32_t fileSize(QString fileName) const;
	QString getSourceFile() const;
	QString getClassFullName() const;
	QString getClassFlagsName() const;

	void printClassDigest();
	void printConstantPool();
	void printClassBody();
	void printFields();
	void printMethods();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
	std::shared_ptr<ClassFile> classFile;
	QStringList stringList;
};

