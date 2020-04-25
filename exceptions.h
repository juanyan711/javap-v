#pragma once

#include<QString>

namespace jdk {

class JdkException
{
public:
	JdkException(QString, QString , int32_t );

	QString getReason();
	QString getFileName();
	int32_t getLineNumber();

	QString toString();

private:
	QString reason;
	QString fileName;
	int32_t lineNumber;
	
};

#define FILE_INFO __FILE__, __LINE__

#define FINALLY  catch (...) { \
   QMessageBox::warning(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("Î´Öª´íÎó, ÇëÖØÆô")); \
}

}

