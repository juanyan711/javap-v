#pragma once

#include<QString>
#include<memory>
#include<vector>

using std::weak_ptr;
using std::vector;
using std::shared_ptr;
using std::make_shared;

//class摘要,魔数,版本信息,常量池,访问标志，类名称， 父类名称，继承的接口，字段表，方法表，属性表
enum class NodeType {
	Root,
	Inherit,
	FullClass,
	Digest,
	Magic,
	Version,
	ConstantPool,
	AccessFlags,
	ThisClass,
	SuperClass,
	Interfaces,
	Fields,
	Field,
	Methods,
	Method,
	ClassAttributes
};

enum class ItemType {
	Plain
};

class TreeNode {
public:
	QString text;
	int32_t begin = 0x7FFFFFFF;
	int32_t end = 0x80000000;
	weak_ptr<TreeNode> parent;
	mutable vector<shared_ptr<TreeNode>> children;
	void* extra = nullptr;
	NodeType type = NodeType::Inherit;
};

class ListItem {
public:
	QString text;
	int32_t begin = 0x7FFFFFFF;
	int32_t end = 0x80000000;
	void* extra = nullptr;
	ItemType type = ItemType::Plain;
	bool range = false;
};



