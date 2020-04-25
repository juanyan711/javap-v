#pragma once

#include <QDialog>
#include "ui_aboutdialog.h"

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog(bool startui,QWidget *parent = Q_NULLPTR);
	~AboutDialog();

	bool eventFilter(QObject*, QEvent*);

private:
	Ui::AboutDialog ui;
	int pastTime = 0;
	static const int totalTime = 5;
};
