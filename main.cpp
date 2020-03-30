#include "stdafx.h"
#include "javapv.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Javapv w;
	w.show();
	return a.exec();
}
