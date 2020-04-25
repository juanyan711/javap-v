#include "stdafx.h"
#include "typeannotation.h"

using namespace jdk;

jdk::TypePathEntry::TypePathEntry(TypePathEntryKind t):tag(t),arg(0)
{
	if (!(tag == TypePathEntryKind::ARRAY ||
		tag == TypePathEntryKind::INNER_TYPE ||
		tag == TypePathEntryKind::WILDCARD)) {
		throw JdkException(QString::fromLocal8Bit("Invalid TypePathEntryKind: %1").arg(QString::number((uint8_t)tag)), FILE_INFO);
	}
}

jdk::TypePathEntry::TypePathEntry(TypePathEntryKind t, int a):tag(t), arg(a)
{
	if (tag != TypePathEntryKind::TYPE_ARGUMENT) {
		throw JdkException(QString("Invalid TypePathEntryKind: ") + (uint8_t)tag, FILE_INFO);
	}
}

shared_ptr<TypePathEntry> TypePathEntry::fromBinary(uint8_t tag, uint8_t arg)
{
	if (arg != 0 && tag != (int)TypePathEntryKind::TYPE_ARGUMENT) {
		throw new JdkException("Invalid TypePathEntry tag/arg: ", FILE_INFO);
	}
	switch (tag) {
	case 0:
		return TypePathEntry::ARRAY;
	case 1:
		return TypePathEntry::INNER_TYPE;
	case 2:
		return TypePathEntry::WILDCARD;
	case 3:
		return shared_ptr<TypePathEntry>(new TypePathEntry(TypePathEntryKind::TYPE_ARGUMENT, arg));
	default:
		throw JdkException(QString("Invalid TypePathEntryKind tag: ") + (uint8_t)tag, FILE_INFO);
	}
}

QString jdk::TypePathEntry::toString()  const
{
	QString buffer;
	switch (tag) {
	case TypePathEntryKind::ARRAY:
		buffer.append("ARRAY");
		break;
	case TypePathEntryKind::INNER_TYPE:
		buffer.append("INNER_TYPE");
		break;
	case TypePathEntryKind::WILDCARD:
		buffer.append("WILDCARD");
		break;
	case TypePathEntryKind::TYPE_ARGUMENT:
		buffer.append("TYPE_ARGUMENT");
		break;
	default:
		throw JdkException(QString("Invalid TypePathEntryKind tag: ") + (uint8_t)tag, FILE_INFO);
	}
	if (tag == TypePathEntryKind::TYPE_ARGUMENT) {
		buffer.append(QString("(")).append(QString::number(arg)).append(")");
	}
	return buffer;
}

const shared_ptr<TypePathEntry> TypePathEntry::ARRAY = shared_ptr<TypePathEntry>(new TypePathEntry(TypePathEntryKind::ARRAY));
const shared_ptr<TypePathEntry> TypePathEntry::INNER_TYPE = shared_ptr<TypePathEntry>(new TypePathEntry(TypePathEntryKind::INNER_TYPE));
const shared_ptr<TypePathEntry> TypePathEntry::WILDCARD = shared_ptr<TypePathEntry>(new TypePathEntry(TypePathEntryKind::WILDCARD));

TargetType::TargetType(int t):targetTypeValue(t), local(false)
{
	if (this->targetTypeValue < 0
		|| this->targetTypeValue > 255)
		throw JdkException(QString("Attribute type value needs to be an unsigned byte: ") + QString::number(targetTypeValue), FILE_INFO);
}

TargetType::TargetType(int t, bool l):targetTypeValue(t),local(l)
{
	if (this->targetTypeValue < 0
		|| this->targetTypeValue > 255)
		throw JdkException(QString("Attribute type value needs to be an unsigned byte: ") + QString::number(targetTypeValue), FILE_INFO);
}

bool jdk::TargetType::isLocal()
{
	return this->local;
}

int jdk::TargetType::getTargetTypeValue()
{
	return this->targetTypeValue;
}

bool jdk::TargetType::isValidTargetTypeValue(int tag)
{
	if (tag == UNKNOWN)
		return true;
	return (tag >= 0 && tag < MAXIMUM_TARGET_TYPE_VALUE + 1);
}

QString jdk::TargetType::toString()
{
	int tag = targetTypeValue;
	if (tag == TargetType::CLASS_TYPE_PARAMETER) {
		return "CLASS_TYPE_PARAMETER";
	}
	if (tag == TargetType::METHOD_TYPE_PARAMETER) {
		return "METHOD_TYPE_PARAMETER";
	}
	if (tag == TargetType::CLASS_EXTENDS) {
		return "CLASS_EXTENDS";
	}
	if (tag == TargetType::CLASS_TYPE_PARAMETER_BOUND) {
		return "CLASS_TYPE_PARAMETER_BOUND";
	}
	if (tag == TargetType::METHOD_TYPE_PARAMETER_BOUND) {
		return "METHOD_TYPE_PARAMETER_BOUND";
	}
	if (tag == TargetType::FIELD) {
		return "FIELD";
	}
	if (tag == TargetType::METHOD_RETURN) {
		return "METHOD_RETURN";
	}
	if (tag == TargetType::METHOD_RECEIVER) {
		return "METHOD_RECEIVER";
	}
	if (tag == TargetType::METHOD_FORMAL_PARAMETER) {
		return "METHOD_FORMAL_PARAMETER";
	}
	if (tag == TargetType::THROWS) {
		return "THROWS";
	}
	if (tag == TargetType::LOCAL_VARIABLE) {
		return "LOCAL_VARIABLE";
	}
	if (tag == TargetType::RESOURCE_VARIABLE) {
		return "RESOURCE_VARIABLE";
	}
	if (tag == TargetType::EXCEPTION_PARAMETER) {
		return "EXCEPTION_PARAMETER";
	}
	if (tag == TargetType::INSTANCEOF) {
		return "INSTANCEOF";
	}
	if (tag == TargetType::NEW) {
		return "NEW";
	}
	if (tag == TargetType::CONSTRUCTOR_REFERENCE) {
		return "CONSTRUCTOR_REFERENCE";
	}
	if (tag == TargetType::METHOD_REFERENCE) {
		return "METHOD_REFERENCE";
	}
	if (tag == TargetType::CAST) {
		return "CAST";
	}
	if (tag == TargetType::CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT) {
		return "CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT";
	}
	if (tag == TargetType::METHOD_INVOCATION_TYPE_ARGUMENT) {
		return "METHOD_INVOCATION_TYPE_ARGUMENT";
	}
	if (tag == TargetType::METHOD_INVOCATION_TYPE_ARGUMENT) {
		return "METHOD_INVOCATION_TYPE_ARGUMENT";
	}
	if (tag == TargetType::CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT) {
		return "CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT";
	}
	if (tag == TargetType::METHOD_REFERENCE_TYPE_ARGUMENT) {
		return "METHOD_REFERENCE_TYPE_ARGUMENT";
	}

	return "UNKNOWN";
}

shared_ptr<TargetType> jdk::TargetType::fromTargetTypeValue(int tag)
{
	if (tag == TargetType::CLASS_TYPE_PARAMETER) {
		static shared_ptr<TargetType> u00(new TargetType(TargetType::CLASS_TYPE_PARAMETER));
		return u00;
	}
	if (tag == TargetType::METHOD_TYPE_PARAMETER) {
		static shared_ptr<TargetType> u01(new TargetType(TargetType::METHOD_TYPE_PARAMETER));
		return u01;
	}
	if (tag == TargetType::CLASS_EXTENDS) {
		static shared_ptr<TargetType> u10(new TargetType(TargetType::CLASS_EXTENDS));
		return u10;
	}
	if (tag == TargetType::CLASS_TYPE_PARAMETER_BOUND) {
		static shared_ptr<TargetType> u11(new TargetType(TargetType::CLASS_TYPE_PARAMETER_BOUND));
		return u11;
	}
	if (tag == TargetType::METHOD_TYPE_PARAMETER_BOUND) {
		static shared_ptr<TargetType> u12(new TargetType(TargetType::METHOD_TYPE_PARAMETER_BOUND));
		return u12;
	}
	if (tag == TargetType::FIELD) {
		static shared_ptr<TargetType> u13(new TargetType(TargetType::FIELD));
		return u13;
	}
	if (tag == TargetType::METHOD_RETURN) {
		static shared_ptr<TargetType> u14(new TargetType(TargetType::METHOD_RETURN));
		return u14;
	}
	if (tag == TargetType::METHOD_RECEIVER) {
		static shared_ptr<TargetType> u15(new TargetType(TargetType::METHOD_RECEIVER));
		return u15;
	}
	if (tag == TargetType::METHOD_FORMAL_PARAMETER) {
		static shared_ptr<TargetType> u16(new TargetType(TargetType::METHOD_FORMAL_PARAMETER));
		return u16;
	}
	if (tag == TargetType::THROWS) {
		static shared_ptr<TargetType> u17(new TargetType(TargetType::THROWS));
		return u17;
	}
	if (tag == TargetType::LOCAL_VARIABLE) {
		static shared_ptr<TargetType> u40(new TargetType(TargetType::LOCAL_VARIABLE));
		return u40;
	}
	if (tag == TargetType::RESOURCE_VARIABLE) {
		static shared_ptr<TargetType> u41(new TargetType(TargetType::RESOURCE_VARIABLE));
		return u41;
	}
	if (tag == TargetType::EXCEPTION_PARAMETER) {
		static shared_ptr<TargetType> u42(new TargetType(TargetType::EXCEPTION_PARAMETER));
		return u42;
	}
	if (tag == TargetType::INSTANCEOF) {
		static shared_ptr<TargetType> u43(new TargetType(TargetType::INSTANCEOF));
		return u43;
	}
	if (tag == TargetType::NEW) {
		static shared_ptr<TargetType> u44(new TargetType(TargetType::NEW));
		return u44;
	}
	if (tag == TargetType::CONSTRUCTOR_REFERENCE) {
		static shared_ptr<TargetType> u45(new TargetType(TargetType::CONSTRUCTOR_REFERENCE));
		return u45;
	}
	if (tag == TargetType::METHOD_REFERENCE) {
		static shared_ptr<TargetType> u46(new TargetType(TargetType::METHOD_REFERENCE));
		return u46;
	}
	if (tag == TargetType::CAST) {
		static shared_ptr<TargetType> u47(new TargetType(TargetType::CAST));
		return u47;
	}
	if (tag == TargetType::CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT) {
		static shared_ptr<TargetType> u48(new TargetType(TargetType::CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT));
		return u48;
	}
	if (tag == TargetType::METHOD_INVOCATION_TYPE_ARGUMENT) {
		static shared_ptr<TargetType> u49(new TargetType(TargetType::METHOD_INVOCATION_TYPE_ARGUMENT));
		return u49;
	}
	if (tag == TargetType::METHOD_INVOCATION_TYPE_ARGUMENT) {
		static shared_ptr<TargetType> u49(new TargetType(TargetType::METHOD_INVOCATION_TYPE_ARGUMENT));
		return u49;
	}
	if (tag == TargetType::CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT) {
		static shared_ptr<TargetType> u4A(new TargetType(TargetType::CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT));
		return u4A;
	}
	if (tag == TargetType::METHOD_REFERENCE_TYPE_ARGUMENT) {
		static shared_ptr<TargetType> u4B(new TargetType(TargetType::METHOD_REFERENCE_TYPE_ARGUMENT));
		return u4B;
	}
	static shared_ptr<TargetType> uff(new TargetType(TargetType::UNKNOWN));
	return uff;
}

jdk::Position::Position()
{
	type = TargetType::fromTargetTypeValue(TargetType::UNKNOWN);
}

QString jdk::Position::toString()
{
	QString sb;
	sb.append(type->toString());

	switch (type->getTargetTypeValue()) {
		// instanceof
	case TargetType::INSTANCEOF:
	case TargetType::NEW:
	case TargetType::CONSTRUCTOR_REFERENCE:
	case TargetType::METHOD_REFERENCE:
		sb.append(", offset = ");
		sb.append(QString::number(offset));
		break;
	case TargetType::LOCAL_VARIABLE:
	case TargetType::RESOURCE_VARIABLE:
		if (lvarOffset.size()==0) {
			sb.append(", lvarOffset is null!");
			break;
		}
		sb.append(", {");
		for (int i = 0; i < lvarOffset.size(); ++i) {
			if (i != 0) sb.append("; ");
			sb.append("start_pc = ");
			sb.append(QString::number(lvarOffset[i]));
			sb.append(", length = ");
			sb.append(QString::number(lvarLength[i]));
			sb.append(", index = ");
			sb.append(QString::number(lvarIndex[i]));
		}
		sb.append("}");
		break;
	case TargetType::METHOD_RECEIVER:
		break;
	case TargetType::CLASS_TYPE_PARAMETER:
	case TargetType::METHOD_TYPE_PARAMETER:
		sb.append(", param_index = ");
		sb.append(QString::number(parameter_index));
		break;
	case TargetType::CLASS_TYPE_PARAMETER_BOUND:
	case TargetType::METHOD_TYPE_PARAMETER_BOUND:
		sb.append(", param_index = ");
		sb.append(QString::number(parameter_index));
		sb.append(", bound_index = ");
		sb.append(QString::number(bound_index));
		break;
	case TargetType::CLASS_EXTENDS:
		sb.append(", type_index = ");
		sb.append(QString::number(type_index));
		break;
	case TargetType::THROWS:
		sb.append(", type_index = ");
		sb.append(QString::number(type_index));
		break;
	case TargetType::EXCEPTION_PARAMETER:
		sb.append(", exception_index = ");
		sb.append(QString::number(exception_index));
		break;
	case TargetType::METHOD_FORMAL_PARAMETER:
		sb.append(", param_index = ");
		sb.append(QString::number(parameter_index));
		break;
	case TargetType::CAST:
	case TargetType::CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT:
	case TargetType::METHOD_INVOCATION_TYPE_ARGUMENT:
	case TargetType::CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT:
	case TargetType::METHOD_REFERENCE_TYPE_ARGUMENT:
		sb.append(", offset = ");
		sb.append(QString::number(offset));
		sb.append(", type_index = ");
		sb.append(QString::number(type_index));
		break;
	case TargetType::METHOD_RETURN:
	case TargetType::FIELD:
		break;
	case TargetType::UNKNOWN:
		sb.append(", position UNKNOWN!");
		break;
	default:
		throw JdkException("Unknown target type: ", FILE_INFO);
	}

	if (!location.size()==0) {
		sb.append(", location = [ ");

		auto itr = location.cbegin();
		bool first = true;
		while (true) {
			if (itr == location.cend()) {
				break;
			}
			if ( !first) {
				sb.append(", ");
			}
			else {
				first = false;
			}
			sb.append((*itr)->toString());
			itr++;
		}

		sb.append(" ]");
	}

	//sb.append(", pos = ");
	//sb.append(pos);

	return sb;
}

bool jdk::Position::emitToClassfile()
{
	return !type->isLocal() || isValidOffset;
}

shared_ptr<Position> Position::read_position(const char* b, uint32_t& length)
{
	// Copied from ClassReader
	int tag = *b;  // TargetType tag is a byte
	uint32_t nextOffset = 1;
	if (!TargetType::isValidTargetTypeValue(tag))
		throw JdkException("TypeAnnotation: Invalid type annotation target type value: ", FILE_INFO);
	auto type = TargetType::fromTargetTypeValue(tag);

	shared_ptr<Position> position(new Position);
	position->type = type;

	switch (type->getTargetTypeValue()) {
	case TargetType::INSTANCEOF:
	case TargetType::NEW:
	case TargetType::CONSTRUCTOR_REFERENCE:
	case TargetType::METHOD_REFERENCE:
		position->offset = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		break;
	case TargetType::LOCAL_VARIABLE:
	case TargetType::RESOURCE_VARIABLE:
	{
		int table_length = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		for (int i = 0; i < table_length; ++i) {
			position->lvarOffset.push_back(Util::readUnsignedInt16(b, nextOffset));
			position->lvarLength.push_back(Util::readUnsignedInt16(b, nextOffset + 2));
			position->lvarIndex.push_back(Util::readUnsignedInt16(b, nextOffset + 4));
			nextOffset += 6;
		}
	}
		break;
	case TargetType::EXCEPTION_PARAMETER:
		position->exception_index = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		break;
	case TargetType::METHOD_RECEIVER:
		break;
	case TargetType::CLASS_TYPE_PARAMETER:
	case TargetType::METHOD_TYPE_PARAMETER:
		position->parameter_index = *(b + nextOffset);
		nextOffset += 1;
		break;
		// type parameter bound
	case TargetType::CLASS_TYPE_PARAMETER_BOUND:
	case TargetType::METHOD_TYPE_PARAMETER_BOUND:
		position->parameter_index = *(b + nextOffset);
		position->bound_index = *(b + nextOffset + 1);
		nextOffset += 2;
		break;
		// class extends or implements clause
	case TargetType::CLASS_EXTENDS:
		position->type_index = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		break;
		// throws
	case TargetType::THROWS:
		position->type_index = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		break;
		// method parameter
	case TargetType::METHOD_FORMAL_PARAMETER:
		position->parameter_index = *(b + nextOffset);
		nextOffset += 1;
		break;
	case TargetType::CAST:
	case TargetType::CONSTRUCTOR_INVOCATION_TYPE_ARGUMENT:
	case TargetType::METHOD_INVOCATION_TYPE_ARGUMENT:
	case TargetType::CONSTRUCTOR_REFERENCE_TYPE_ARGUMENT:
	case TargetType::METHOD_REFERENCE_TYPE_ARGUMENT:
		position->offset = Util::readUnsignedInt16(b, nextOffset);
		nextOffset += 2;
		position->type_index = *(b + nextOffset);
		nextOffset += 1;
		break;
	case TargetType::METHOD_RETURN:
	case TargetType::FIELD:
		break;
	case TargetType::UNKNOWN:
		throw JdkException("TypeAnnotation: UNKNOWN target type should never occur!", FILE_INFO);
	default:
		throw JdkException("TypeAnnotation: Unknown target type. ", FILE_INFO);
	}

	int len = *(b+nextOffset);
	nextOffset += 1;
	vector<int> loc;
	for (int i = 0; i < len ; ++i) {
		uint8_t kind = *(b + nextOffset);
		uint8_t arg = *(b + nextOffset + 1);
		position->location.push_back(TypePathEntry::fromBinary(kind, arg));
		nextOffset += 2;
	}

	length = nextOffset;
	return position;
}

