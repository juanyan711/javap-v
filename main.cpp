#include "stdafx.h"
#include "javapv.h"
#include <QtWidgets/QApplication>
#include "aboutdialog.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AboutDialog dialog(true);
	dialog.setModal(true);
	dialog.exec();

	Javapv w;
	w.show();
	return a.exec();
}
