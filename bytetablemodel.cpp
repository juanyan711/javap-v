#include "stdafx.h"
#include "bytetablemodel.h"

ByteTableModel::ByteTableModel(const ClassFile* cf, int32_t c, QObject* parent) :columns(c), 
                                                                                 classFile(cf), 
	                                                                             QAbstractTableModel(parent),
																				 outBegin(0xFFFFFFFF), outEnd(-1), innerBegin(0xFFFFFFFF), innerEnd(-1)
{
}


QVariant ByteTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	int place = (index.row()) * columns + index.column()+1;
	if (place > classFile->getBufferSize()) {
		return QVariant();
	}
	if (Qt::TextAlignmentRole == role)
	{
		QVariant value = int(Qt::AlignCenter | Qt::AlignHCenter);
		return value;
	}
	else if (Qt::BackgroundRole == role) {
		uint32_t row = index.row();
		uint32_t col = index.column();
		int32_t offset = row * columns + col;
		if (offset >= outBegin && offset <= outEnd) {
			return QColor(Qt::yellow);
		}
	}
	else if (Qt::ForegroundRole == role) {
		uint32_t row = index.row();
		uint32_t col = index.column();
		int32_t offset = row * columns + col;
		if (offset >= innerBegin && offset <= innerEnd) {
			return QColor(Qt::red);
		}
	}
	else if (Qt::DisplayRole == role && classFile != nullptr) {
		int row = index.row();
		int column = index.column();

		int index = row * 10 + column;
		
		const char* buffer = classFile->getBuffer();
		const char c = buffer[index];
		int s = 0x000000FF & c;
		return QString("%1").arg(s, 2, 16, QLatin1Char('0'));
	}
	return QVariant();
}

QVariant ByteTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) {
		return QVariant();
	}
	if (orientation == Qt::Orientation::Horizontal) {
		return QString("%1").arg(section, 2, 16, QLatin1Char('0'));
	}
	return QVariant();
}

int ByteTableModel::columnCount(const QModelIndex& parent) const
{
	if (!parent.isValid()) {
		return columns;
	}
	return 0;
}

int ByteTableModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid() && classFile!=nullptr) {
		uint32_t bufferSize = classFile->getBufferSize();
		uint32_t row = bufferSize / columns;
		if (bufferSize % columns != 0) {
			row += 1;
		}
		return row;
	}
	return 0;
}

void ByteTableModel::setOutOffset(int32_t begin, int32_t end)
{
	outBegin = begin;
	outEnd = end;
	innerBegin = 0xFFFFFFFF;
	innerEnd = -1;
	
	int brow = outBegin / columns;
	int bcolumn = outBegin % columns;
	auto bindex = index(brow, bcolumn);

	int erow = outEnd / columns;
	int ecolumn = outEnd % columns;
	auto eindex = index(erow, ecolumn);

	emit dataChanged(bindex, eindex);
}

void ByteTableModel::setInnerOffset(int32_t begin, int32_t end)
{
	if (begin<outBegin || end > outEnd) {
		throw JdkException("inner offset invalid", FILE_INFO);
	}
	innerBegin = begin;
	innerEnd = end;
}

QModelIndex ByteTableModel::calcIndex(uint32_t offset)
{
	int row = outBegin / columns;
	int column = outBegin % columns;
	return index(row, column);
}
