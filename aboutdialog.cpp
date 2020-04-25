#include "stdafx.h"
#include "aboutdialog.h"
#include<QTimer>

AboutDialog::AboutDialog(bool startui, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//Ìí¼ÓÒ»¸öÍ¼±êÍ¼Æ¬
	QLabel* ico = new QLabel(this);
	ico->resize(160, 200);
	QRect rect = ico->geometry();
	rect.setTop(50);
	rect.setLeft(20);
	ico->setGeometry(rect);
	QPixmap pix(":/images/Resources/images/coffe.jpg");
	QPixmap dest = pix.scaled(ico->size(), Qt::KeepAspectRatio);
	ico->setPixmap(dest);

	//°ó¶¨url
	connect(ui.gitlink, &QLabel::linkActivated, this, [=](QString url) {
		QDesktopServices::openUrl(QUrl(url));
	});

	ui.lblEmail->installEventFilter(this);

	// Qt::ToolÒþ²Ø¹¤¾ßÀ¸Í¼±ê
	// Qt::FramelessWindowHint Òþ²Ø±ß¿ò
	// Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint  ¶¥²ã´°¿Ú
	if(startui){
		this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint| Qt::Tool);

		ui.btnClose->hide();
		QString text = QString::fromLocal8Bit("%1ÃëºóÌø×ª").arg(AboutDialog::totalTime);
		this->ui.downTime->setText(text);
		ui.downTime->show();

		QTimer* timer = new QTimer(this);
		timer->start(1000);

		connect(timer, &QTimer::timeout, this, [=]() {
			this->pastTime++;
			this->ui.downTime->show();
			QString text = QString::fromLocal8Bit("%1ÃëºóÌø×ª").arg(AboutDialog::totalTime - this->pastTime);
			this->ui.downTime->setText(text);
			if (this->pastTime > AboutDialog::totalTime-1) {
				this->close();
			}
		});
	}
	else {
		this->setWindowFlags(Qt::X11BypassWindowManagerHint |  Qt::FramelessWindowHint | Qt::Tool);
		ui.downTime->hide();
		ui.btnClose->show();

		connect(ui.btnClose, &QPushButton::clicked, this, [=]() {
			this->close();
		});
	}
}

AboutDialog::~AboutDialog()
{
}

bool AboutDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
		if (mouse->button() == Qt::RightButton) {
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText("juanyan711@163.com");
		}
	}
	return false;
}
