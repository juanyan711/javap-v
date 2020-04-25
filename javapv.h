#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_javapv.h"
#include "constantPool.h"
#include "classfile.h"
#include "ClassFileModel.h"
#include "ClassTreeModel.h"
#include "bytetablemodel.h"

#include<QFile>

using namespace jdk;
using std::shared_ptr;
using std::make_shared;


class Javapv : public QMainWindow
{
	Q_OBJECT

public:
	Javapv(QWidget *parent = Q_NULLPTR);
	void closeEvent(QCloseEvent* event);
	

protected:
	void bindEvent();
private:
	std::shared_ptr<ClassFile> loadClassFile(QString fileName = QString::fromUtf8(u8"D:\\ideas\\j8\\asm\\target\\classes\\com\\thy\\annos\\\cls\\StackMap.class"));
	void resetByteViewWidth();
	void clear();
	void showJdkExceptionMessage(JdkException& ex);


private slots:

private:
	Ui::javapvClass ui;

	QString lastPath;

	shared_ptr<ClassFileModel> detailModel;
	shared_ptr<ClassTreeModel> treeModel;
	shared_ptr<ByteTableModel> byteModel;
	shared_ptr<ClassFile> classFile;

	QMenu* vViewMenu;

	bool checkJdkVersion = true;
	static const int byteViewColumns = 10;
};
