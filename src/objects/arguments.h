// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_ARGUMENTS_H_
#define V8_OBJECTS_ARGUMENTS_H_

#include "src/objects.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

// Common superclass for JSSloppyArgumentsObject and JSStrictArgumentsObject.
class JSArgumentsObject : public JSObject {
 public:
  // Offsets of object fields.
  static const int kLengthOffset = JSObject::kHeaderSize;
  static const int kHeaderSize = kLengthOffset + kPointerSize;
  // Indices of in-object properties.
  static const int kLengthIndex = 0;

  DECL_ACCESSORS(length, Object)

  DECLARE_VERIFIER(JSArgumentsObject)
  DECLARE_CAST(JSArgumentsObject)

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(JSArgumentsObject);
};

// JSSloppyArgumentsObject is just a JSObject with specific initial map.
// This initial map adds in-object properties for "length" and "callee".
class JSSloppyArgumentsObject : public JSArgumentsObject {
 public:
  // Offsets of object fields.
  static const int kCalleeOffset = JSArgumentsObject::kHeaderSize;
  static const int kSize = kCalleeOffset + kPointerSize;
  // Indices of in-object properties.
  static const int kCalleeIndex = kLengthIndex + 1;

  DECL_ACCESSORS(callee, Object)

  DECLARE_VERIFIER(JSSloppyArgumentsObject)
  DECLARE_CAST(JSSloppyArgumentsObject)

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(JSSloppyArgumentsObject);
};

// JSStrictArgumentsObject is just a JSObject with specific initial map.
// This initial map adds an in-object property for "length".
class JSStrictArgumentsObject : public JSArgumentsObject {
 public:
  // Offsets of object fields.
  static const int kSize = JSArgumentsObject::kHeaderSize;

  DECLARE_CAST(JSStrictArgumentsObject)

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(JSStrictArgumentsObject);
};

// Helper class to access FAST_ and SLOW_SLOPPY_ARGUMENTS_ELEMENTS
//
// +---+-----------------------+
// | 0 | Context* context      |
// +---------------------------+
// | 1 | FixedArray* arguments +----+ FAST_HOLEY_ELEMENTS
// +---------------------------+    v-----+-----------+
// | 2 | Object* param_1_map   |    |  0  | the_hole  |
// |...| ...                   |    | ... | ...       |
// |n+1| Object* param_n_map   |    | n-1 | the_hole  |
// +---------------------------+    |  n  | element_1 |
//                                  | ... | ...       |
//                                  |n+m-1| element_m |
//                                  +-----------------+
//
// Parameter maps give the index into the provided context. If a map entry is
// the_hole it means that the given entry has been deleted from the arguments
// object.
// The arguments backing store kind depends on the ElementsKind of the outer
// JSArgumentsObject:
// - FAST_SLOPPY_ARGUMENTS_ELEMENTS: FAST_HOLEY_ELEMENTS
// - SLOW_SLOPPY_ARGUMENTS_ELEMENTS: DICTIONARY_ELEMENTS
class SloppyArgumentsElements : public FixedArray {
 public:
  static const int kContextIndex = 0;
  static const int kArgumentsIndex = 1;
  static const uint32_t kParameterMapStart = 2;

  inline Context* context();
  inline FixedArray* arguments();
  inline void set_arguments(FixedArray* arguments);
  inline uint32_t parameter_map_length();
  inline Object* get_mapped_entry(uint32_t entry);
  inline void set_mapped_entry(uint32_t entry, Object* object);

  DECLARE_CAST(SloppyArgumentsElements)
#ifdef VERIFY_HEAP
  void SloppyArgumentsElementsVerify(JSSloppyArgumentsObject* holder);
#endif

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(SloppyArgumentsElements);
};

// Representation of a slow alias as part of a sloppy arguments objects.
// For fast aliases (if HasSloppyArgumentsElements()):
// - the parameter map contains an index into the context
// - all attributes of the element have default values
// For slow aliases (if HasDictionaryArgumentsElements()):
// - the parameter map contains no fast alias mapping (i.e. the hole)
// - this struct (in the slow backing store) contains an index into the context
// - all attributes are available as part if the property details
class AliasedArgumentsEntry : public Struct {
 public:
  inline int aliased_context_slot() const;
  inline void set_aliased_context_slot(int count);

  DECLARE_CAST(AliasedArgumentsEntry)

  // Dispatched behavior.
  DECLARE_PRINTER(AliasedArgumentsEntry)
  DECLARE_VERIFIER(AliasedArgumentsEntry)

  static const int kAliasedContextSlot = HeapObject::kHeaderSize;
  static const int kSize = kAliasedContextSlot + kPointerSize;

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(AliasedArgumentsEntry);
};

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_ARGUMENTS_H_
