//===-- Implementation of vsnprintf -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/vsnprintf.h"

#include "src/__support/arg_list.h"
#include "src/__support/macros/config.h"
#include "src/stdio/printf_core/printf_main.h"
#include "src/stdio/printf_core/writer.h"

#include <stdarg.h>
#include <stddef.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, vsnprintf,
                   (char *__restrict buffer, size_t buffsz,
                    const char *__restrict format, va_list vlist)) {
  internal::ArgList args(vlist); // This holder class allows for easier copying
                                 // and pointer semantics, as well as handling
                                 // destruction automatically.
  printf_core::WriteBuffer<printf_core::Mode<
      printf_core::WriteMode::FILL_BUFF_AND_DROP_OVERFLOW>::value>
      wb(buffer, (buffsz > 0 ? buffsz - 1 : 0));
  printf_core::Writer writer(wb);

  int ret_val = printf_core::printf_main(&writer, format, args);
  if (buffsz > 0) // if the buffsz is 0 the buffer may be a null pointer.
    wb.buff[wb.buff_cur] = '\0';
  return ret_val;
}

} // namespace LIBC_NAMESPACE_DECL
