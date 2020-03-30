#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_javapv.h"
#include "constantPool.h"
#include "classfile.h"
#include "ClassFileModel.h"

#include<QFile>

using namespace jdk;

class Javapv : public QMainWindow
{
	Q_OBJECT

public:
	Javapv(QWidget *parent = Q_NULLPTR);

protected:
	void bindEvent();
private:
	std::shared_ptr<ClassFile> loadClassFile(QString fileName = QString::fromUtf8(u8"D:\\ideas\\j8\\asm\\target\\classes\\com\\thy\\annos\\\cls\\Student.class"));

private slots:

private:
	Ui::javapvClass ui;
	QAbstractItemModel* vModel;
	std::vector<std::shared_ptr<ClassFile>> classFiles;
	int currentIndex;
};
