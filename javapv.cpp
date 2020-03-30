#include "stdafx.h"
#include "javapv.h"

Javapv::Javapv(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//this->setWindowState(Qt::WindowState::WindowMaximized);   //初始最大化

	bindEvent();
}

std::shared_ptr<ClassFile> Javapv::loadClassFile(QString fileName)
{
	QFile  file(fileName);
	if (!file.exists()) {
		qDebug() << "file \"" << fileName << "\" not exists!";
		return nullptr;
	}
	file.open(QIODevice::ReadOnly);
	QByteArray src = file.readAll();
	file.close();

	std::shared_ptr<ClassFile> pf(new ClassFile(fileName, src));

	return pf;

}

void Javapv::bindEvent()
{
	//vModel的捕获方式问题
	connect(ui.action_test, &QAction::triggered, this, [pvModel=&vModel, ui=this->ui, that = this](bool trigged) {
		Q_UNUSED(trigged)

		QString signature = QString::fromUtf8(u8"(TT;Lcom/thy/annos/cls/Student<TT;TW;TC;>.Inner;)Lcom/thy/annos/cls/Student<TT;TW;TC;>.Inner;");
		auto mVisitor = std::make_shared<MethodSignatureVisitor>();
		if (!signature.isEmpty()) {
			SignatureReader sReader(signature);
			sReader.accept(mVisitor);
			qDebug()<<"method return value:"<<mVisitor->getRetureSignature();
			qDebug() << "method parameter value:" << mVisitor->getParamSignature();
			qDebug() << "method throws value:" << mVisitor->getThrowsSignature();
		}

	});

	connect(ui.action_open, &QAction::triggered, this, [pvModel = &vModel, ui = this->ui, that = this](bool trigged) {
		Q_UNUSED(trigged)
			qDebug() << QString::fromUtf8(u8"action_test clicked");

		// TODO :选择文件

		std::shared_ptr<ClassFile> pf = that->loadClassFile();

		*pvModel = new ClassFileModel(pf, that);
		ui.vView->setModel(that->vModel);

	});
}


