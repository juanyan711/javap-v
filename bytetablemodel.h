#pragma once
#include <qabstractitemmodel.h>
#include "classFile.h"

using namespace jdk;
using std::shared_ptr;
using std::weak_ptr;

class ByteTableModel :
	public QAbstractTableModel
{
public:
	ByteTableModel(const ClassFile* cf, int32_t columns = 16, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int columnCount(const QModelIndex& parent) const;
	int rowCount(const QModelIndex& parent) const;
	void setOutOffset(int32_t begin, int32_t end);
	void setInnerOffset(int32_t begin, int32_t end);

	QModelIndex  calcIndex(uint32_t offset);
private:
	const ClassFile* classFile;
	uint16_t columns;
	int32_t outBegin;
	int32_t outEnd;
	int32_t innerBegin;
	int32_t innerEnd;
};

