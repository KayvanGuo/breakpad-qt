// Copyright (c) 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// BasicSourceLineResolver implements SourceLineResolverInterface, using
// address map files produced by a compatible writer, e.g. PDBSourceLineWriter.

#ifndef GOOGLE_BREAKPAD_PROCESSOR_BASIC_SOURCE_LINE_RESOLVER_H__
#define GOOGLE_BREAKPAD_PROCESSOR_BASIC_SOURCE_LINE_RESOLVER_H__

// TODO: Platforms that have no hash_map can use map, at the likely cost of
// performance.
#ifdef __SUNPRO_CC
#define BSLR_NO_HASH_MAP
#endif  // __SUNPRO_CC

#ifdef BSLR_NO_HASH_MAP
#include <map>
#else  // BSLR_NO_HASH_MAP
#include <ext/hash_map>
#endif  // BSLR_NO_HASH_MAP

#include "google_breakpad/processor/source_line_resolver_interface.h"

namespace google_breakpad {

using std::string;
#ifdef BSLR_NO_HASH_MAP
using std::map;
#else  // BSLR_NO_HASH_MAP
using __gnu_cxx::hash_map;
#endif  // BSLR_NO_HASH_MAP

class BasicSourceLineResolver : public SourceLineResolverInterface {
 public:
  BasicSourceLineResolver();
  virtual ~BasicSourceLineResolver();

  // SourceLineResolverInterface methods, see source_line_resolver_interface.h
  // for more details.

  // Adds a module to this resolver, returning true on success.
  // The given map_file is read into memory, and its symbols will be
  // retained until the BasicSourceLineResolver is destroyed.
  virtual bool LoadModule(const string &module_name, const string &map_file);

  // Exactly the same as above, except the given map_buffer is used
  // for symbols. 
  virtual bool LoadModuleUsingMapBuffer(const string &module_name,
                                        const string &map_buffer);


  virtual bool HasModule(const string &module_name) const;

  virtual StackFrameInfo* FillSourceLineInfo(StackFrame *frame) const;

 private:
  template<class T> class MemAddrMap;
  struct Line;
  struct Function;
  struct PublicSymbol;
  struct File;
#ifdef BSLR_NO_HASH_MAP
  struct CompareString {
    bool operator()(const string &s1, const string &s2) const;
  };
#else  // BSLR_NO_HASH_MAP
  struct HashString {
    size_t operator()(const string &s) const;
  };
#endif  // BSLR_NO_HASH_MAP
  class Module;

  // All of the modules we've loaded
#ifdef BSLR_NO_HASH_MAP
  typedef map<string, Module*, CompareString> ModuleMap;
#else  // BSLR_NO_HASH_MAP
  typedef hash_map<string, Module*, HashString> ModuleMap;
#endif  // BSLR_NO_HASH_MAP
  ModuleMap *modules_;

  // Disallow unwanted copy ctor and assignment operator
  BasicSourceLineResolver(const BasicSourceLineResolver&);
  void operator=(const BasicSourceLineResolver&);
};

}  // namespace google_breakpad

#endif  // GOOGLE_BREAKPAD_PROCESSOR_BASIC_SOURCE_LINE_RESOLVER_H__
