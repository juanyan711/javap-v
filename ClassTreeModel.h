#pragma once

#include<QAbstractItemModel>
#include "classFile.h"
#include "viewcommon.h"

using namespace jdk;

class ClassTreeModel : public QAbstractItemModel
{
	Q_OBJECT
	// Í¨¹ý QAbstractItemModel ¼Ì³Ð
public:
	ClassTreeModel(const ClassFile* , QObject* parent = nullptr);
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
	const ClassFile* classFile;
	shared_ptr<TreeNode> root;
};

