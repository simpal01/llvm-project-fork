set(CFI_TESTSUITES)

macro (add_cfi_test_suites lld thinlto)
  set(suffix)
  if (${lld})
    set(suffix ${suffix}-lld)
  endif()
  if (${thinlto})
    set(suffix ${suffix}-thinlto)
  endif()
  set(suffix ${suffix}-${CFI_TEST_TARGET_ARCH})

  set(CFI_TEST_USE_LLD ${lld})
  set(CFI_TEST_USE_THINLTO ${thinlto})

  set(CFI_LIT_TEST_MODE Standalone)
  set(CFI_TEST_CONFIG_SUFFIX -standalone${suffix})
  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/Standalone${suffix}/lit.site.cfg.py
    )
  list(APPEND CFI_TESTSUITES ${CMAKE_CURRENT_BINARY_DIR}/Standalone${suffix})

  set(CFI_LIT_TEST_MODE Devirt)
  set(CFI_TEST_CONFIG_SUFFIX -devirt${suffix})
  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/Devirt${suffix}/lit.site.cfg.py
    )
  list(APPEND CFI_TESTSUITES ${CMAKE_CURRENT_BINARY_DIR}/Devirt${suffix})
endmacro()

set(CFI_TEST_ARCH ${CFI_SUPPORTED_ARCH})
if(APPLE)
  darwin_filter_host_archs(CFI_SUPPORTED_ARCH CFI_TEST_ARCH)
endif()

foreach(arch ${CFI_TEST_ARCH})
  set(CFI_TEST_TARGET_ARCH ${arch})
  get_test_cc_for_arch(${arch} CFI_TEST_TARGET_CC CFI_TEST_TARGET_CFLAGS)
  if (APPLE)
    # FIXME: enable ThinLTO tests after fixing http://llvm.org/pr32741
    add_cfi_test_suites(False False False)
  elseif(WIN32)
    add_cfi_test_suites(True False)
    add_cfi_test_suites(True True)
  else()
    add_cfi_test_suites(False False)
    add_cfi_test_suites(False True)
    if (COMPILER_RT_HAS_LLD)
	    add_cfi_test_suites(True False)
	    add_cfi_test_suites(True True)
    endif()
  endif()
endforeach()

set(CFI_TEST_DEPS ${SANITIZER_COMMON_LIT_TEST_DEPS})
list(APPEND CFI_TEST_DEPS
  ubsan
  stats
)
list(APPEND CFI_TEST_DEPS cfi)

if(NOT COMPILER_RT_STANDALONE_BUILD)
  list(APPEND CFI_TEST_DEPS
    opt
    sanstats
  )
  if(LLVM_ENABLE_PIC)
    if(LLVM_BINUTILS_INCDIR)
      list(APPEND CFI_TEST_DEPS
        LLVMgold
      )
    endif()
    if(APPLE)
      list(APPEND CFI_TEST_DEPS
        LTO
      )
    endif()
  endif()
  if(NOT APPLE AND COMPILER_RT_HAS_LLD AND TARGET lld)
    list(APPEND CFI_TEST_DEPS
      lld
    )
  endif()
endif()

add_lit_testsuite(check-cfi "Running the cfi regression tests"
  ${CFI_TESTSUITES}
  DEPENDS ${CFI_TEST_DEPS})

add_lit_target(check-cfi-and-supported "Running the cfi regression tests"
  ${CFI_TESTSUITES}
  PARAMS check_supported=1
  DEPENDS ${CFI_TEST_DEPS})
