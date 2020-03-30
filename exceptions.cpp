#include "stdafx.h"
#include "exceptions.h"

using namespace jdk;
JdkException::JdkException(QString rs, QString fn, int32_t ln):reason(rs), 
															   fileName(fn), 
															   lineNumber(ln)
{
}

QString JdkException::getReason()
{
	return this->reason;
}

QString JdkException::getFileName()
{
	return this->fileName;
}

int32_t JdkException::getLineNumber()
{
	return this->lineNumber;
}

QString JdkException::toString()
{
	QString str = QString::fromUtf8(u8" file:[ %1 ], lineNumber: [ %2 ],reason: [ %3 ] ")
		                  .arg(fileName)
						  .arg(lineNumber)
						  .arg(reason);
	return str;
}
