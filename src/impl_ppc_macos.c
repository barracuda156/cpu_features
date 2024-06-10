// Copyright 2017 Google LLC
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

#include "cpu_features_macros.h"

#ifdef CPU_FEATURES_ARCH_PPC
#ifdef CPU_FEATURES_OS_MACOS

#include "impl_ppc__base_implementation.inl"

#if !defined(HAVE_SYSCTLBYNAME)
#error "Darwin needs support for sysctlbyname"
#endif
#include <sys/sysctl.h>

static int GetDarwinSysCtlByNameValue(const char* name) {
  int enabled;
  size_t enabled_len = sizeof(enabled);
  const int failure = sysctlbyname(name, &enabled, &enabled_len, NULL, 0);
  return failure ? 0 : enabled;
}

static bool GetDarwinSysCtlByName(const char* name) {
  return GetDarwinSysCtlByNameValue(name) != 0;
}

static const PPCInfo kEmptyPPCInfo;

PPCInfo GetPPCInfo(void) {
  PPCInfo info = kEmptyPPCInfo;

  // Handling Darwin platform through sysctlbyname.
  info.cputype = GetDarwinSysCtlByNameValue("hw.cputype");
  info.cpusubtype = GetDarwinSysCtlByNameValue("hw.cpusubtype");
  info.cpufamily = GetDarwinSysCtlByNameValue("hw.cpufamily");

  info.floatingpoint = GetDarwinSysCtlByName("hw.optional.floatingpoint");
  info.altivec = GetDarwinSysCtlByName("hw.optional.altivec");
  info.graphicsops = GetDarwinSysCtlByName("hw.optional.graphicsops");
  info.64bitops = GetDarwinSysCtlByName("hw.optional.64bitops");
  info.fsqrt = GetDarwinSysCtlByName("hw.optional.fsqrt");
  info.stfiwx = GetDarwinSysCtlByName("hw.optional.stfiwx");
  info.dcba = GetDarwinSysCtlByName("hw.optional.dcba");
  info.datastreams = GetDarwinSysCtlByName("hw.optional.datastreams");
  info.dcbtstreams = GetDarwinSysCtlByName("hw.optional.dcbtstreams");

  return info;
}

#endif  // CPU_FEATURES_OS_MACOS
#endif  // CPU_FEATURES_ARCH_PPC
