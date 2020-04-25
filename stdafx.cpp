#include "stdafx.h"

using namespace jdk;

template class U4Info<uint32_t, ConstantTag::CONSTANT_INTEGER>;
template class U4Info<float, ConstantTag::CONSTANT_FLOAT>;

template class U8Info<uint64_t, ConstantTag::CONSTANT_LONG>;
template class U8Info<double, ConstantTag::CONSTANT_DOUBLE>;