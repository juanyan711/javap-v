#include "stdafx.h"
#include "javapv.h"

#include "aboutdialog.h"

Javapv::Javapv(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.action_check->setChecked(checkJdkVersion);

	ui.vView->setContextMenuPolicy(Qt::CustomContextMenu);

	vViewMenu = new QMenu(this);
	vViewMenu->addAction(ui.action_copy);

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
	pf->load(checkJdkVersion);

	return pf;

}

void Javapv::resetByteViewWidth()
{
	auto fontMetrics = ui.bView->fontMetrics();
	int pixWidth = fontMetrics.width("ee") + 12;
	qDebug() << "pix width:" << pixWidth;
	int width = pixWidth * byteViewColumns + 20;
	ui.bView->setFixedWidth(width);
}

void Javapv::clear()
{
	this->ui.vView->setModel(nullptr);
	this->ui.nTree->setModel(nullptr);
	this->ui.bView->setModel(nullptr);

	this->detailModel = nullptr;
	this->treeModel = nullptr;
	this->byteModel = nullptr;

	this->classFile = nullptr;

	this->ui.statusBar->showMessage("");
}

void Javapv::showJdkExceptionMessage(JdkException& ex)
{
	QString errorText = QString::fromLocal8Bit("%1 \r\n FileInfo: %2  %3 ").arg(ex.getReason()).arg(ex.getFileName()).arg(ex.getLineNumber());
	QMessageBox::warning(this, QString::fromLocal8Bit("错误"), errorText);
}

void Javapv::closeEvent(QCloseEvent* event)
{
	int button  = QMessageBox::question(this, QString::fromLocal8Bit("退出"),
		QString(QString::fromLocal8Bit("确认退出程序?")),
		QMessageBox::Yes | QMessageBox::No);
	if (button == QMessageBox::No) {
		event->ignore();
	}
	else if (button == QMessageBox::Yes) {
		event->accept(); 
	}
}

void Javapv::bindEvent()
{
	//测试代码
	connect(ui.action_test, &QAction::triggered, this, [=](bool trigged) {
		Q_UNUSED(trigged)

	});

	connect(ui.action_exit, &QAction::triggered, this, [=](bool trigged) {
		this->close();
	});

	//关闭文件
	connect(ui.action_close, &QAction::triggered, this, [=](bool) {
		clear();
	});

	//加载文件
	connect(ui.action_open, &QAction::triggered, this, [=](bool trigged) {
		Q_UNUSED(trigged)

		QString fileName = QFileDialog::getOpenFileName(
			this,
			tr("open a class"),
			lastPath,
			tr("classes(*.class);;All files(*.*)"));
		if (fileName.isEmpty()) {
			return;
		}
		this->lastPath = QFileInfo(fileName).path();

		try {
			auto pf = this->loadClassFile(fileName);
			if (pf == nullptr) {
				return;
			}

			auto detail = make_shared<ClassFileModel>(pf.get(), this);
			auto tree = make_shared<ClassTreeModel>(pf.get(), this);
			auto byte = make_shared<ByteTableModel>(pf.get(), byteViewColumns, this);

			this->ui.vView->setModel(detail.get());

			this->ui.nTree->setModel(tree.get());
			this->ui.nTree->expandAll();
			this->ui.nTree->resizeColumnToContents(0);

			this->ui.bView->setModel(byte.get());

			this->detailModel = detail;
			this->treeModel = tree;
			this->byteModel = byte;

			this->classFile = pf;

			this->ui.statusBar->showMessage(this->classFile->getFileName());
		}
		catch (JdkException& ex) {
			clear();
			showJdkExceptionMessage(ex);
		}
		FINALLY
	});

	connect(ui.nTree, &QTreeView::clicked, this, [=](const QModelIndex& index) {
		if (index.isValid()) {
			try {
				TreeNode* node = static_cast<TreeNode*>(index.internalPointer());
				TreeNode* viewNode = node;
				this->ui.bView->clearSelection();
				if (node->type == NodeType::Digest) {
					this->byteModel->setOutOffset(node->begin, node->end);
				}
				else if (node->type == NodeType::Inherit) {
					auto out = node->parent.lock();
					while (out->type == NodeType::Inherit) {
						out = out->parent.lock();
					}
					if (out->type == NodeType::Root) {
						throw JdkException("out node type is root", FILE_INFO);
					}
					this->byteModel->setOutOffset(out->begin, out->end);
					this->byteModel->setInnerOffset(node->begin, node->end);
					viewNode = out.get();
				}
				else {
					this->byteModel->setOutOffset(node->begin, node->end);
				}
				this->detailModel->resetNodeType(viewNode->type, viewNode->extra);
				if (node->type == NodeType::Digest) {
					this->ui.bView->scrollToTop();
				}
				else {
					this->ui.bView->scrollTo(this->byteModel->calcIndex(node->begin));
				}
			}
			catch (JdkException & ex) {
				showJdkExceptionMessage(ex);
			}
			FINALLY
		}
	});

	connect(ui.vView, &QListView::customContextMenuRequested, this, [=](const QPoint& pos) {
		if (this->ui.vView->indexAt(pos).isValid()) {
			vViewMenu->exec(QCursor::pos());
		}
	});

	connect(ui.action_copy, &QAction::triggered, this, [=](bool trigged) {
		auto listView = this->ui.vView;
		auto index = listView->currentIndex();
		ListItem*  item = static_cast<ListItem*>(index.internalPointer());
		QString text = item->text.trimmed();
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(text);
	});

	connect(ui.action_about, &QAction::triggered, this, [=](bool trigged) {
		AboutDialog dialog(false, this);
		dialog.exec();
	});

	connect(ui.action_check, &QAction::triggered, this, [=](bool trigged) {
		this->checkJdkVersion = trigged;
	});
}


