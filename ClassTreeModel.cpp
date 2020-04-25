#include "stdafx.h"
#include "ClassTreeModel.h"


ClassTreeModel::ClassTreeModel(const ClassFile* cf, QObject* parent) :classFile(cf),root(new TreeNode()), QAbstractItemModel(parent) {
	if (classFile == nullptr) {
		return; 
	}

	root->type = NodeType::Root;

	//class摘要,魔数,版本信息,常量池,访问标志，类名称， 父类名称，继承的接口，字段表，方法表，属性表
	auto node1 = make_shared<TreeNode>();
	node1->text = QString::fromLocal8Bit("class摘要");
	node1->parent = root;
	node1->type = NodeType::Digest;

	auto magic = make_shared<TreeNode>();
	magic->text = QString::fromLocal8Bit("魔数");
	magic->begin = classFile->getOffset();
	magic->end = classFile->getOffset() + 3;
	magic->parent = root;
	magic->type = NodeType::Magic;

	auto version = make_shared<TreeNode>();
	version->text = QString::fromLocal8Bit("版本信息");
	version->begin = magic->end + 1;
	version->end = version->begin + 3;
	version->parent = root;
	version->type = NodeType::Version;

	auto pool = make_shared<TreeNode>();
	pool->text = QString::fromLocal8Bit("常量池");
	pool->begin = version->end + 1;
	pool->end = pool->begin + classFile->getConstantPool()->getLength() - 1;
	pool->parent = root;
	pool->type = NodeType::ConstantPool;

	auto classFlags = make_shared<TreeNode>();
	classFlags->text = QString::fromLocal8Bit("类访问标志");
	classFlags->begin = pool->end + 1;
	classFlags->end = classFlags->begin + 1;
	classFlags->parent = root;
	classFlags->type = NodeType::AccessFlags;

	auto thisClass = make_shared<TreeNode>();
	thisClass->text = QString::fromLocal8Bit("This类");
	thisClass->begin = classFlags->end + 1;
	thisClass->end = thisClass->begin + 1;
	thisClass->parent = root;
	thisClass->type = NodeType::ThisClass;

	auto superClass = make_shared<TreeNode>();
	superClass->text = QString::fromLocal8Bit("继承父类");
	superClass->begin = thisClass->end + 1;
	superClass->end = superClass->begin + 1;
	superClass->parent = root;
	superClass->type = NodeType::SuperClass;

	auto interfaces = make_shared<TreeNode>();
	interfaces->text = QString::fromLocal8Bit("实现接口");
	interfaces->begin = superClass->end + 1;
	interfaces->end = interfaces->begin + 2 + classFile->getInterfaceCount() * 2 - 1;
	interfaces->parent = root;
	interfaces->type = NodeType::Interfaces;

	auto fields = make_shared<TreeNode>();
	fields->text = QString::fromLocal8Bit("字段");
	fields->begin = interfaces->end + 1;
	fields->parent = root;
	fields->type = NodeType::Fields;

	auto& fieldInfos = classFile->getFields();
	int32_t fieldNextOffset = fields->begin + 2;
	for (auto fieldInfo : fieldInfos) {
		auto field = make_shared<TreeNode>();
		field->text = fieldInfo->getName();
		field->begin = fieldNextOffset;
		field->end = fieldNextOffset + fieldInfo->getLength() - 1;
		field->parent = fields;
		field->type = NodeType::Field;
		field->extra = fieldInfo.get();

		fields->children.push_back(field);

		fieldNextOffset = field->end + 1;

		auto& attributeInfos = fieldInfo->getAttributesRef();
		int32_t attributeNextOffset = field->begin + 8;
		for (auto attributeInfo : attributeInfos) {
			auto attribute = make_shared<TreeNode>();
			attribute->text = attributeInfo->getAttributeName();
			attribute->begin = attributeNextOffset;
			attribute->end = attributeNextOffset + attributeInfo->getBodyLength() + 6 - 1;
			attribute->parent = field;
			attribute->type = NodeType::Inherit;

			field->children.push_back(attribute);
			attributeNextOffset = attribute->end + 1;
		}
	}
	fields->end = fieldNextOffset - 1;

	auto methods = make_shared<TreeNode>();
	methods->text = QString::fromLocal8Bit("方法");
	methods->begin = fields->end + 1;
	methods->parent = root;
	methods->type = NodeType::Methods;

	auto& methodInfos = classFile->getMethods();
	int32_t methodNextOffset = methods->begin + 2;
	for (auto methodInfo : methodInfos) {
		auto method = make_shared<TreeNode>();
		method->text = methodInfo->getName();
		method->begin = methodNextOffset;
		method->end = methodNextOffset + methodInfo->getLength() - 1;
		method->parent = methods;
		method->type = NodeType::Method;
		method->extra = methodInfo.get();

		methods->children.push_back(method);

		methodNextOffset = method->end + 1;

		auto& attributeInfos = methodInfo->getAttributesRef();
		int32_t attributeNextOffset = method->begin + 8;
		for (auto attributeInfo : attributeInfos) {
			auto attribute = make_shared<TreeNode>();
			attribute->text = attributeInfo->getAttributeName();
			attribute->begin = attributeNextOffset;
			attribute->end = attributeNextOffset + attributeInfo->getBodyLength() + 6 - 1;
			attribute->parent = method;
			attribute->type = NodeType::Inherit;

			method->children.push_back(attribute);
			attributeNextOffset = attribute->end + 1;

			if (QString::compare(attributeInfo->getAttributeName(), "Code") == 0) {
				auto code = methodInfo->getCodeAttribute();
				auto& codeAttributeInfos = code->getAttributes();
				int32_t codeAttributeNextOffset = attribute->begin + 14 + code->getCodeLength() + 2 + code->getExceptionTableLength() * 8 + 2;
				for (auto codeAttributeInfo : codeAttributeInfos) {
					auto codeAttribute = make_shared<TreeNode>();
					codeAttribute->text = codeAttributeInfo->getAttributeName();
					codeAttribute->begin = codeAttributeNextOffset;
					codeAttribute->end = codeAttributeNextOffset + codeAttributeInfo->getBodyLength() + 6 - 1;
					codeAttribute->parent = attribute;
					codeAttribute->type = NodeType::Inherit;

					attribute->children.push_back(codeAttribute);
					codeAttributeNextOffset = codeAttribute->end + 1;

				}
			}
		}
	}
	methods->end = methodNextOffset - 1;

	auto attributes = make_shared<TreeNode>();
	attributes->text = QString::fromLocal8Bit("属性");
	attributes->begin = methods->end + 1;
	attributes->parent = root;
	attributes->type = NodeType::ClassAttributes;

	auto& attributeInfos = classFile->getAttributes();
	int32_t attributeNextOffset = attributes->begin + 2;
	for (auto attributeInfo : attributeInfos) {
		auto attribute = make_shared<TreeNode>();
		attribute->text = attributeInfo->getAttributeName();
		attribute->begin = attributeNextOffset;
		attribute->end = attributeNextOffset + attributeInfo->getBodyLength() + 6 - 1;
		attribute->parent = attributes;
		attribute->type = NodeType::Inherit;

		attributes->children.push_back(attribute);
		attributeNextOffset = attribute->end + 1;
	}
	attributes->end = attributeNextOffset - 1;

	root->children.push_back(node1);
	root->children.push_back(magic);
	root->children.push_back(version);
	root->children.push_back(pool);
	root->children.push_back(classFlags);
	root->children.push_back(thisClass);
	root->children.push_back(superClass);
	root->children.push_back(interfaces);
	root->children.push_back(fields);
	root->children.push_back(methods);
	root->children.push_back(attributes);
}

QModelIndex ClassTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid()) {
		if (row < root->children.size()) {
			return createIndex(row, column, root->children[row].get());
		}
	}
	else {
		TreeNode* node = static_cast<TreeNode*>(parent.internalPointer());
		if (row < node->children.size()) {
			return createIndex(row, column, node->children[row].get());
		}
	}
	return QModelIndex();
}

QModelIndex ClassTreeModel::parent(const QModelIndex& child) const
{
	if (child.isValid() && child.internalPointer() != nullptr) {
		TreeNode* node = static_cast<TreeNode*>(child.internalPointer());
		auto parent = node->parent.lock();
		if (parent != root) {
			auto grant = parent->parent.lock();
			for (uint32_t i = 0; i < grant->children.size(); i++) {
				if (QString::compare(grant->children[i]->text, parent->text) == 0) {
					return createIndex(i, child.column(), grant->children[i].get());
				}
			}
		}
	}
	return QModelIndex();
}

int ClassTreeModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid()) {
		return root->children.size();
	}
	else {
		TreeNode* node = static_cast<TreeNode*>(parent.internalPointer());
		return node->children.size();
	}
}

int ClassTreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant ClassTreeModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole) {
		TreeNode* node = static_cast<TreeNode*>(index.internalPointer());
		return node->text;
	}
	else if (role == Qt::ToolTipRole) {
		TreeNode* node = static_cast<TreeNode*>(index.internalPointer());
		return "";
	}
	return QVariant();
}
