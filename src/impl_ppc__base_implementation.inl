// Copyright 2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>

#include "cpu_features_macros.h"
#include "cpuinfo_ppc.h"
#include "internal/bit_utils.h"
#include "internal/filesystem.h"
#include "internal/stack_line_reader.h"
#include "internal/string_view.h"

#if !defined(CPU_FEATURES_ARCH_PPC)
#error "Cannot use PowerPC defs on a non-PowerPC platform."
#endif

////////////////////////////////////////////////////////////////////////////////
// Definitions for introspection.
////////////////////////////////////////////////////////////////////////////////
#define INTROSPECTION_TABLE \
  LINE(PPC_32, ppc32, "ppc32", PPC_HWCAP_32, 0) \
  LINE(PPC_64, ppc64, "ppc64", PPC_HWCAP_64, 0) \
  LINE(PPC_601_INSTR, ppc601, "ppc601", PPC_HWCAP_601_INSTR, 0) \
  LINE(PPC_HAS_ALTIVEC, altivec, "altivec", PPC_HWCAP_ALTIVEC, 0) \
  LINE(PPC_HAS_FPU, fpu, "fpu", PPC_HWCAP_FPU, 0) \
  LINE(PPC_HAS_MMU, mmu, "mmu", PPC_HWCAP_MMU, 0) \
  LINE(PPC_HAS_4xxMAC, 4xxmac, "4xxmac", PPC_HWCAP_4xxMAC, 0) \
  LINE(PPC_UNIFIED_CACHE, unified_cache, "unified_cache", PPC_HWCAP_UNIFIED_CACHE, 0) \
  LINE(PPC_HAS_SPE, spe, "spe", PPC_HWCAP_SPE, 0) \
  LINE(PPC_HAS_EFP_SINGLE, efp_single, "efp_single", PPC_HWCAP_EFP_SINGLE, 0) \
  LINE(PPC_HAS_EFP_DOUBLE, efp_double, "efp_double", PPC_HWCAP_EFP_DOUBLE, 0) \
  LINE(PPC_NO_TB, no_tb, "no_tb", PPC_HWCAP_NO_TB, 0) \
  LINE(PPC_G5, g5, "g5", PPC_HWCAP_G5, 0) \
  LINE(PPC_POWER4, power4, "power4", PPC_HWCAP_POWER4, 0) \
  LINE(PPC_POWER5, power5, "power5", PPC_HWCAP_POWER5, 0) \
  LINE(PPC_POWER5_PLUS, power5_plus, "power5_plus", PPC_HWCAP_POWER5_PLUS, 0) \
  LINE(PPC_CELL, cell, "cell", PPC_HWCAP_CELL, 0) \
  LINE(PPC_BOOKE, booke, "booke", PPC_HWCAP_BOOKE, 0) \
  LINE(PPC_SMT, smt, "smt", PPC_HWCAP_SMT, 0) \
  LINE(PPC_ICACHE_SNOOP, icache_snoop, "icache_snoop", PPC_HWCAP_ICACHE_SNOOP, 0) \
  LINE(PPC_ARCH_2_05, arch_2_05, "arch_2_05", PPC_HWCAP_ARCH_2_05, 0) \
  LINE(PPC_PA6T, pa6t, "pa6t", PPC_HWCAP_PA6T, 0) \
  LINE(PPC_HAS_DFP, dfp, "dfp", PPC_HWCAP_HAS_DFP, 0) \
  LINE(PPC_POWER6_EXT, power6_ext, "power6_ext", PPC_HWCAP_POWER6_EXT, 0) \
  LINE(PPC_ARCH_2_06, isa_2_06, "isa_2_06", PPC_HWCAP_ARCH_2_06, 0) \
  LINE(PPC_HAS_VSX, vsx, "vsx", PPC_HWCAP_VSX, 0) \
  LINE(PPC_PSERIES_PERFMON_COMPAT, perfmon_compat, "perfmon_compat", PPC_HWCAP_PSERIES_PERFMON_COMPAT, 0) \
  LINE(PPC_TRUE_LE, true_le, "true_le", PPC_HWCAP_TRUE_LE, 0) \
  LINE(PPC_PPC_LE, le, "le", PPC_HWCAP_PPC_LE, 0) \
  LINE(PPC_ARCH_2_07, isa_2_07, "isa_2_07", PPC_HWCAP_ARCH_2_07, 0) \
  LINE(PPC_HTM, htm, "htm", PPC_HWCAP_HTM, 0) \
  LINE(PPC_DSCR, dscr, "dscr", PPC_HWCAP_DSCR, 0) \
  LINE(PPC_EBB, ebb, "ebb", 0, PPC_HWCAP_EBB, 0) \
  LINE(PPC_ISEL, isel, "isel", 0, PPC_HWCAP_ISEL, 0) \
  LINE(PPC_TAR, tar, "tar", 0, PPC_TAR, 0) \
  LINE(PPC_VEC_CRYPTO, crypto, "crypto", 0, PPC_HWCAP_VEC_CRYPTO, 0) \
  LINE(PPC_HTM_NOSC, htm_nosc, "htm_nosc", 0, PPC_HWCAP_HTM_NOSC, 0) \
  LINE(PPC_ARCH_3_00, arch_3_00, "arch_3_00", 0, PPC_HWCAP_ARCH_3_00, 0) \
  LINE(PPC_HAS_IEEE128, ieee128, "ieee128", 0, PPC_HWCAP_IEEE128, 0) \
  LINE(PPC_DARN, darn, "darn", 0, PPC_HWCAP_DARN, 0) \
  LINE(PPC_SCV, scv, "scv", 0, PPC_HWCAP_SCV, 0) \
  LINE(PPC_HTM_NO_SUSPEND, htm_no_suspend, "htm_no_suspend", 0, PPC_HWCAP_HTM_NO_SUSPEND, 0)
#define INTROSPECTION_PREFIX PPC
#define INTROSPECTION_ENUM_PREFIX PPC
#include "define_introspection_and_hwcaps.inl"
