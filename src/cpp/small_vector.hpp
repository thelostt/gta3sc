/// Vector with small stack storage.
///
/// http://llvm.org/docs/doxygen/html/classllvm_1_1SmallVector.html
///
#pragma once
#include <SmallVector/SmallVector.h>

template <typename T, unsigned N>
using small_vector = llvm_vecsmall::SmallVector<T, N>;
