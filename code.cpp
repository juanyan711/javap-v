#include "stdafx.h"
#include "code.h"

using std::make_tuple;
using std::unique_ptr;
using std::get;
using namespace jdk;

//b代表字节码占据的空间
//c(const，字面量)代表后面接一个字节的直接参数, cc代表后面接两个字节的直接参数
//k(constant的发音)代表后面接一个字节的常量池索引, kk代表后面接两个字节的常量池索引
//i(index)代表一字节的局部变量表和操作数表的索引
//o(offset)代表跳转的偏移量
//j代表NameAndType的常量池索引
const array<tuple<int32_t, QString, QString, QString>, 255>  jdk::Bytecodes::INSTRUCTIONS = {
make_tuple(_nop, "nop", "b", ""),
make_tuple(_aconst_null         , "aconst_null"         , "b"    , ""),
make_tuple(_iconst_m1           , "iconst_m1"           , "b"    , ""),
make_tuple(_iconst_0            , "iconst_0"            , "b"    , ""),
make_tuple(_iconst_1            , "iconst_1"            , "b"    , ""),
make_tuple(_iconst_2            , "iconst_2"            , "b"    , ""),
make_tuple(_iconst_3            , "iconst_3"            , "b"    , ""),
make_tuple(_iconst_4            , "iconst_4"            , "b"    , ""),
make_tuple(_iconst_5            , "iconst_5"            , "b"    , ""),
make_tuple(_lconst_0            , "lconst_0"            , "b"    , ""),
make_tuple(_lconst_1            , "lconst_1"            , "b"    , ""),
make_tuple(_fconst_0            , "fconst_0"            , "b"    , ""),
make_tuple(_fconst_1            , "fconst_1"            , "b"    , ""),
make_tuple(_fconst_2            , "fconst_2"            , "b"    , ""),
make_tuple(_dconst_0            , "dconst_0"            , "b"    , ""),
make_tuple(_dconst_1            , "dconst_1"            , "b"    , ""),
make_tuple(_bipush              , "bipush"              , "bc"   , ""),
make_tuple(_sipush              , "sipush"              , "bcc"  , ""),
make_tuple(_ldc                 , "ldc"                 , "bk"   , ""),
make_tuple(_ldc_w               , "ldc_w"               , "bkk"  , ""),
make_tuple(_ldc2_w              , "ldc2_w"              , "bkk"  , ""),
make_tuple(_iload               , "iload"               , "bi"   , "wbii"),
make_tuple(_lload               , "lload"               , "bi"   , "wbii"),
make_tuple(_fload               , "fload"               , "bi"   , "wbii"),
make_tuple(_dload               , "dload"               , "bi"   , "wbii"),
make_tuple(_aload               , "aload"               , "bi"   , "wbii"),
make_tuple(_iload_0             , "iload_0"             , "b"    , ""),
make_tuple(_iload_1             , "iload_1"             , "b"    , ""),
make_tuple(_iload_2             , "iload_2"             , "b"    , ""),
make_tuple(_iload_3             , "iload_3"             , "b"    , ""),
make_tuple(_lload_0             , "lload_0"             , "b"    , ""),
make_tuple(_lload_1             , "lload_1"             , "b"    , ""),
make_tuple(_lload_2             , "lload_2"             , "b"    , ""),
make_tuple(_lload_3             , "lload_3"             , "b"    , ""),
make_tuple(_fload_0             , "fload_0"             , "b"    , ""),
make_tuple(_fload_1             , "fload_1"             , "b"    , ""),
make_tuple(_fload_2             , "fload_2"             , "b"    , ""),
make_tuple(_fload_3             , "fload_3"             , "b"    , ""),
make_tuple(_dload_0             , "dload_0"             , "b"    , ""),
make_tuple(_dload_1             , "dload_1"             , "b"    , ""),
make_tuple(_dload_2             , "dload_2"             , "b"    , ""),
make_tuple(_dload_3             , "dload_3"             , "b"    , ""),
make_tuple(_aload_0             , "aload_0"             , "b"    , ""),
make_tuple(_aload_1             , "aload_1"             , "b"    , ""),
make_tuple(_aload_2             , "aload_2"             , "b"    , ""),
make_tuple(_aload_3             , "aload_3"             , "b"    , ""),
make_tuple(_iaload              , "iaload"              , "b"    , ""),
make_tuple(_laload              , "laload"              , "b"    , ""),
make_tuple(_faload              , "faload"              , "b"    , ""),
make_tuple(_daload              , "daload"              , "b"    , ""),
make_tuple(_aaload              , "aaload"              , "b"    , ""),
make_tuple(_baload              , "baload"              , "b"    , ""),
make_tuple(_caload              , "caload"              , "b"    , ""),
make_tuple(_saload              , "saload"              , "b"    , ""),
make_tuple(_istore              , "istore"              , "bi"   , "wbii"),
make_tuple(_lstore              , "lstore"              , "bi"   , "wbii"),
make_tuple(_fstore              , "fstore"              , "bi"   , "wbii"),
make_tuple(_dstore              , "dstore"              , "bi"   , "wbii"),
make_tuple(_astore              , "astore"              , "bi"   , "wbii"),
make_tuple(_istore_0            , "istore_0"            , "b"    , ""),
make_tuple(_istore_1            , "istore_1"            , "b"    , ""),
make_tuple(_istore_2            , "istore_2"            , "b"    , ""),
make_tuple(_istore_3            , "istore_3"            , "b"    , ""),
make_tuple(_lstore_0            , "lstore_0"            , "b"    , ""),
make_tuple(_lstore_1            , "lstore_1"            , "b"    , ""),
make_tuple(_lstore_2            , "lstore_2"            , "b"    , ""),
make_tuple(_lstore_3            , "lstore_3"            , "b"    , ""),
make_tuple(_fstore_0            , "fstore_0"            , "b"    , ""),
make_tuple(_fstore_1            , "fstore_1"            , "b"    , ""),
make_tuple(_fstore_2            , "fstore_2"            , "b"    , ""),
make_tuple(_fstore_3            , "fstore_3"            , "b"    , ""),
make_tuple(_dstore_0            , "dstore_0"            , "b"    , ""),
make_tuple(_dstore_1            , "dstore_1"            , "b"    , ""),
make_tuple(_dstore_2            , "dstore_2"            , "b"    , ""),
make_tuple(_dstore_3            , "dstore_3"            , "b"    , ""),
make_tuple(_astore_0            , "astore_0"            , "b"    , ""),
make_tuple(_astore_1            , "astore_1"            , "b"    , ""),
make_tuple(_astore_2            , "astore_2"            , "b"    , ""),
make_tuple(_astore_3            , "astore_3"            , "b"    , ""),
make_tuple(_iastore             , "iastore"             , "b"    , ""),
make_tuple(_lastore             , "lastore"             , "b"    , ""),
make_tuple(_fastore             , "fastore"             , "b"    , ""),
make_tuple(_dastore             , "dastore"             , "b"    , ""),
make_tuple(_aastore             , "aastore"             , "b"    , ""),
make_tuple(_bastore             , "bastore"             , "b"    , ""),
make_tuple(_castore             , "castore"             , "b"    , ""),
make_tuple(_sastore             , "sastore"             , "b"    , ""),
make_tuple(_pop                 , "pop"                 , "b"    , ""),
make_tuple(_pop2                , "pop2"                , "b"    , ""),
make_tuple(_dup                 , "dup"                 , "b"    , ""),
make_tuple(_dup_x1              , "dup_x1"              , "b"    , ""),
make_tuple(_dup_x2              , "dup_x2"              , "b"    , ""),
make_tuple(_dup2                , "dup2"                , "b"    , ""),
make_tuple(_dup2_x1             , "dup2_x1"             , "b"    , ""),
make_tuple(_dup2_x2             , "dup2_x2"             , "b"    , ""),
make_tuple(_swap                , "swap"                , "b"    , ""),
make_tuple(_iadd                , "iadd"                , "b"    , ""),
make_tuple(_ladd                , "ladd"                , "b"    , ""),
make_tuple(_fadd                , "fadd"                , "b"    , ""),
make_tuple(_dadd                , "dadd"                , "b"    , ""),
make_tuple(_isub                , "isub"                , "b"    , ""),
make_tuple(_lsub                , "lsub"                , "b"    , ""),
make_tuple(_fsub                , "fsub"                , "b"    , ""),
make_tuple(_dsub                , "dsub"                , "b"    , ""),
make_tuple(_imul                , "imul"                , "b"    , ""),
make_tuple(_lmul                , "lmul"                , "b"    , ""),
make_tuple(_fmul                , "fmul"                , "b"    , ""),
make_tuple(_dmul                , "dmul"                , "b"    , ""),
make_tuple(_idiv                , "idiv"                , "b"    , ""),
make_tuple(_ldiv                , "ldiv"                , "b"    , ""),
make_tuple(_fdiv                , "fdiv"                , "b"    , ""),
make_tuple(_ddiv                , "ddiv"                , "b"    , ""),
make_tuple(_irem                , "irem"                , "b"    , ""),
make_tuple(_lrem                , "lrem"                , "b"    , ""),
make_tuple(_frem                , "frem"                , "b"    , ""),
make_tuple(_drem                , "drem"                , "b"    , ""),
make_tuple(_ineg                , "ineg"                , "b"    , ""),
make_tuple(_lneg                , "lneg"                , "b"    , ""),
make_tuple(_fneg                , "fneg"                , "b"    , ""),
make_tuple(_dneg                , "dneg"                , "b"    , ""),
make_tuple(_ishl                , "ishl"                , "b"    , ""),
make_tuple(_lshl                , "lshl"                , "b"    , ""),
make_tuple(_ishr                , "ishr"                , "b"    , ""),
make_tuple(_lshr                , "lshr"                , "b"    , ""),
make_tuple(_iushr               , "iushr"               , "b"    , ""),
make_tuple(_lushr               , "lushr"               , "b"    , ""),
make_tuple(_iand                , "iand"                , "b"    , ""),
make_tuple(_land                , "land"                , "b"    , ""),
make_tuple(_ior                 , "ior"                 , "b"    , ""),
make_tuple(_lor                 , "lor"                 , "b"    , ""),
make_tuple(_ixor                , "ixor"                , "b"    , ""),
make_tuple(_lxor                , "lxor"                , "b"    , ""),
make_tuple(_iinc                , "iinc"                , "bic"  , "wbiicc"),
make_tuple(_i2l                 , "i2l"                 , "b"    , ""),
make_tuple(_i2f                 , "i2f"                 , "b"    , ""),
make_tuple(_i2d                 , "i2d"                 , "b"    , ""),
make_tuple(_l2i                 , "l2i"                 , "b"    , ""),
make_tuple(_l2f                 , "l2f"                 , "b"    , ""),
make_tuple(_l2d                 , "l2d"                 , "b"    , ""),
make_tuple(_f2i                 , "f2i"                 , "b"    , ""),
make_tuple(_f2l                 , "f2l"                 , "b"    , ""),
make_tuple(_f2d                 , "f2d"                 , "b"    , ""),
make_tuple(_d2i                 , "d2i"                 , "b"    , ""),
make_tuple(_d2l                 , "d2l"                 , "b"    , ""),
make_tuple(_d2f                 , "d2f"                 , "b"    , ""),
make_tuple(_i2b                 , "i2b"                 , "b"    , ""),
make_tuple(_i2c                 , "i2c"                 , "b"    , ""),
make_tuple(_i2s                 , "i2s"                 , "b"    , ""),
make_tuple(_lcmp                , "lcmp"                , "b"    , ""),
make_tuple(_fcmpl               , "fcmpl"               , "b"    , ""),
make_tuple(_fcmpg               , "fcmpg"               , "b"    , ""),
make_tuple(_dcmpl               , "dcmpl"               , "b"    , ""),
make_tuple(_dcmpg               , "dcmpg"               , "b"    , ""),
make_tuple(_ifeq                , "ifeq"                , "boo"  , ""),
make_tuple(_ifne                , "ifne"                , "boo"  , ""),
make_tuple(_iflt                , "iflt"                , "boo"  , ""),
make_tuple(_ifge                , "ifge"                , "boo"  , ""),
make_tuple(_ifgt                , "ifgt"                , "boo"  , ""),
make_tuple(_ifle                , "ifle"                , "boo"  , ""),
make_tuple(_if_icmpeq           , "if_icmpeq"           , "boo"  , ""),
make_tuple(_if_icmpne           , "if_icmpne"           , "boo"  , ""),
make_tuple(_if_icmplt           , "if_icmplt"           , "boo"  , ""),
make_tuple(_if_icmpge           , "if_icmpge"           , "boo"  , ""),
make_tuple(_if_icmpgt           , "if_icmpgt"           , "boo"  , ""),
make_tuple(_if_icmple           , "if_icmple"           , "boo"  , ""),
make_tuple(_if_acmpeq           , "if_acmpeq"           , "boo"  , ""),
make_tuple(_if_acmpne           , "if_acmpne"           , "boo"  , ""),
make_tuple(_goto                , "goto"                , "boo"  , ""),
make_tuple(_jsr                 , "jsr"                 , "boo"  , ""),
make_tuple(_ret                 , "ret"                 , "bi"   , "wbii"),
make_tuple(_tableswitch         , "tableswitch"         , ""     , ""),
make_tuple(_lookupswitch        , "lookupswitch"        , ""     , ""),
make_tuple(_ireturn             , "ireturn"             , "b"    , ""),
make_tuple(_lreturn             , "lreturn"             , "b"    , ""),
make_tuple(_freturn             , "freturn"             , "b"    , ""),
make_tuple(_dreturn             , "dreturn"             , "b"    , ""),
make_tuple(_areturn             , "areturn"             , "b"    , ""),
make_tuple(_return              , "return"              , "b"    , ""),
make_tuple(_getstatic           , "getstatic"           , "bJJ"  , ""),
make_tuple(_putstatic           , "putstatic"           , "bJJ"  , ""),
make_tuple(_getfield            , "getfield"            , "bJJ"  , ""),
make_tuple(_putfield            , "putfield"            , "bJJ"  , ""),
make_tuple(_invokevirtual       , "invokevirtual"       , "bJJ"  , ""),
make_tuple(_invokespecial       , "invokespecial"       , "bJJ"  , ""),
make_tuple(_invokestatic        , "invokestatic"        , "bJJ"  , ""),
make_tuple(_invokeinterface     , "invokeinterface"     , "bJJ__", ""),
make_tuple(_invokedynamic       , "invokedynamic"       , "bJJJJ", ""),
make_tuple(_new                 , "new"                 , "bkk"  , ""),
make_tuple(_newarray            , "newarray"            , "bc"   , ""),
make_tuple(_anewarray           , "anewarray"           , "bkk"  , ""),
make_tuple(_arraylength         , "arraylength"         , "b"    , ""),
make_tuple(_athrow              , "athrow"              , "b"    , ""),
make_tuple(_checkcast           , "checkcast"           , "bkk"  , ""),
make_tuple(_instanceof          , "instanceof"          , "bkk"  , ""),
make_tuple(_monitorenter        , "monitorenter"        , "b"    , ""),
make_tuple(_monitorexit         , "monitorexit"         , "b"    , ""),
make_tuple(_wide                , "wide"                , ""     , ""),
make_tuple(_multianewarray      , "multianewarray"      , "bkkc" , ""),
make_tuple(_ifnull              , "_ifnull"             , "boo"  , ""),
make_tuple(_ifnonnull           , "_ifnonnull"          , "boo"  , ""),
make_tuple(_goto_w              , "goto_w"              , "boooo", ""),
make_tuple(_jsr_w               , "jsr_w"               , "boooo", ""),
make_tuple(_breakpoint          , "breakpoint"          , ""     , ""),

make_tuple(_shouldnotreachhere  , "_shouldnotreachhere" , "b"    , "")

};

jdk::CodeAttribute::CodeAttribute(const AttributeInfo* c, const ConstantPool* cp):code(c),cpool(cp)
{
	code->visit([=](shared_ptr<const AttributeInfo> info) {
		const char* buffer = info->getBody();
		this->maxStack = Util::readUnsignedInt16(buffer, 0);
		this->maxlocals = Util::readUnsignedInt16(buffer, 2);
		this->codeLength = Util::readUnsignedInt32(buffer, 4);
		this->codeStart = 8;
		uint32_t exceptionOffset =  8 + this->codeLength;
		this->exceptionTableLength = Util::readUnsignedInt16(buffer, exceptionOffset);
		uint32_t attributeOffset = exceptionOffset + 2 + 8 * this->exceptionTableLength;
		this->attributeCount = Util::readUnsignedInt16(buffer, attributeOffset);

		uint32_t nextAttributeOffset = attributeOffset + 2;
		for (uint32_t i = 0; i < this->attributeCount; i++) {
			auto attribute = AttributeInfo::createAttributeInfo(buffer, nextAttributeOffset, this->cpool);
			this->attributes.push_back(attribute);
			nextAttributeOffset += attribute->getBodyLength() + 6;   //attribute_name(u2)   attribute_length(u4)
		}
	});
}

const AttributeInfo* jdk::CodeAttribute::getCode()
{
	// TODO: 在此处插入 return 语句
	return this->code;
}

uint16_t jdk::CodeAttribute::getMaxStack() const
{
	return this->maxStack;
}

uint16_t jdk::CodeAttribute::getMaxLocals() const
{
	return this->maxlocals;
}

uint32_t jdk::CodeAttribute::getCodeLength() const
{
	return this->codeLength;
}

uint16_t jdk::CodeAttribute::getExceptionTableLength() const
{
	return this->exceptionTableLength;
}

const vector<shared_ptr<jdk::AttributeInfo>>& jdk::CodeAttribute::getAttributes() const
{
	// TODO: 在此处插入 return 语句
	return this->attributes;
}

void jdk::CodeAttribute::each(CodeVisitor& visitor)
{
	const char* b = code->getBody();
	uint32_t u = codeStart;
	uint32_t codeEnd = codeLength + codeStart;    //计算结束偏移量
	while (u < codeEnd) {
		int offset = u - codeStart;     //每次都重新计算偏移
		InstructionContext context;
		context.offset = offset;
		uint8_t opcode = b[u];   // b[u] & 0xFF;   //字节码
		u++;
		bool isWide = false;
		auto& tp = Bytecodes::INSTRUCTIONS[opcode];
		int32_t code = get<0>(tp);
		if (code != opcode) {
			throw JdkException("opcode error", FILE_INFO);
		}
		QString codeName = get<1>(tp);
		QString formatter = get<2>(tp);

		if (QString::compare("wide", codeName) == 0) {
			qDebug() << "wide 指令";
			context.wide = isWide = true;
			opcode = b[u];
			u++;
			auto& tp = Bytecodes::INSTRUCTIONS[opcode];
			code = get<0>(tp);
			if (code != opcode) {
				throw JdkException("opcode error", FILE_INFO);
			}
			codeName = get<1>(tp);
			formatter = get<3>(tp);

			if (formatter.isEmpty() || formatter.size() == 0) {
				context.remark = " [ Wide is not allowed ] ";
			}
		}

		context.opcode = opcode;
		context.codeName = codeName;

		if (formatter.isEmpty() || formatter.size()==0) {
			//排除code == wide
			if (code == Bytecodes::_tableswitch) {
				u += 3 - (offset & 3);    //结尾处应该是按4对齐的
				uint32_t default = offset + Util::readInt32(b + u, 0);
				int32_t low = Util::readInt32(b + u, 4);
				int32_t high = Util::readInt32(b + u, 8);
				u += 12;
				int32_t tableLength = high - low + 1;
				shared_ptr<vector<int32_t>> table = std::make_shared<vector<int32_t>>();
				for (int32_t i = 0; i < tableLength; i++) {
					int32_t delta = offset + Util::readInt32(b + u, 0);
					table->push_back(delta);
					u += 4;
				}
				table->push_back(default);
				context.extra = table.get();
				visitor.visit(context);
			}
			else if (code == Bytecodes::_lookupswitch) {
				u += 3 - (offset & 3);   //结尾处应该是按4对齐的
				uint32_t default = offset + Util::readInt32(b + u, 0);
				int nPairs = Util::readInt32(b + u + 4, 0);
				u += 8;
				shared_ptr<map<const QString, int32_t>> lookupTable = shared_ptr<map<const QString, int32_t>>(new map<const QString, int32_t>());
				for (int32_t i = 0; i < nPairs; i++) {
					int32_t  key = Util::readInt32(b + u, 0);
					int32_t  delta = offset + Util::readInt32(b + u + 4, 0);
					lookupTable->insert(map< const QString, int32_t>::value_type(QString::number(key), delta));
					qDebug() << " key:" << key << "  offset: " << delta;
					u += 8;
				}
				lookupTable->insert(map< const QString, int32_t>::value_type("default", default));
				context.extra = lookupTable.get();
				visitor.visit(context);
			}
			else {
				qDebug() << "code name:" << codeName << ", noformatter";
				context.remark += " [ no implement. ]";
				visitor.visit(context);
			}
		}
		else {
			if (isWide) {
				formatter = formatter.mid(2, formatter.size() - 2);
			}
			else {
				formatter = formatter.mid(1, formatter.size() - 1);
			}
			context.formatter = formatter;
			int32_t size = formatter.size();
			int32_t current = u;
			u += size;
			if (size == 0) {
				visitor.visit(context);
				continue;
			}
			else {
				//b代表字节码占据的空间
				//c(const，字面量)代表后面接一个字节的直接参数, cc代表后面接两个字节的直接参数
				//k(constant的发音)代表后面接一个字节的常量池索引, kk代表后面接两个字节的常量池索引
				//i(index)代表一字节的局部变量表和操作数表的索引
				//o(offset)代表跳转的偏移量
				//j代表NameAndType的常量池索引

				if (QString::compare("c", formatter) == 0 ||
					QString::compare("i", formatter) == 0) {
					int8_t p1 = *(b + current);
					context.parameter1 = (context.parameter1 & 0x0) | (uint32_t)(uint8_t)p1;
					visitor.visit(context);
				}
				else if (QString::compare("cc", formatter) == 0 ||
					QString::compare("ii", formatter) == 0) {
					int16_t p1 = Util::readInt16(b, current);
					context.parameter1 = (context.parameter1 & 0x0) | (uint32_t)(uint16_t)p1;
					visitor.visit(context);
				}
				else if (QString::compare("oo", formatter) == 0) {
					int16_t p1 = Util::readInt16(b, current);
					context.parameter1 = (context.parameter1 & 0x0) | (uint32_t)(uint16_t)p1;
					visitor.visit(context);
				}
				else if (QString::compare("JJ", formatter) == 0
					|| QString::compare("kk", formatter) == 0) {
					uint16_t index = Util::readUnsignedInt16(b, current);
					context.parameter1 = (context.parameter1 & 0x0) | (uint32_t)index;
					visitor.visit(context);
				}
				else if (QString::compare("k", formatter) == 0) {
					uint8_t index = *(b + current);
					context.parameter1 = (context.parameter1 & 0x0) | (uint32_t)index;
					visitor.visit(context);
				}
				else if (QString::compare("JJJJ", formatter) == 0 || 
					QString::compare("JJ__", formatter) == 0) {
					context.parameter1 = Util::readUnsignedInt16(b, current);
					if (opcode == Bytecodes::_invokedynamic) {
						context.parameter2 = Util::readUnsignedInt16(b, current + 2);   //invokedynamic固定为0
					}
					else if (opcode == Bytecodes::_invokeinterface) {
						context.parameter2 = 2;
					}
					visitor.visit(context);
				}
				else if (opcode == Bytecodes::_iinc) {
					if (!isWide) {
						context.parameter1 = *(b + current);
						context.parameter2 = *(b + current + 1);
					}
					else {
						context.parameter1 = Util::readUnsignedInt16(b, current);
						context.parameter2 = Util::readUnsignedInt16(b, current + 2);
					}
					visitor.visit(context);
				}
				else if (opcode == Bytecodes::_multianewarray) {
					context.parameter1 = Util::readUnsignedInt16(b, current);
					context.parameter2 = *(b + current + 2);
					visitor.visit(context);
				}
				else if (QString::compare("oooo", formatter) == 0) {
					context.parameter1 = Util::readInt32(b, current);
					visitor.visit(context);
				}
				else {
					qDebug() << "code name:" << codeName << ", formatter: " << formatter;
					context.remark += " [ no implement. ]";
					visitor.visit(context);
				}
			}
		}
	}
}
