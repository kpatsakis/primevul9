void InstanceKlass::do_nonstatic_fields(FieldClosure* cl) {
  InstanceKlass* super = superklass();
  if (super != NULL) {
    super->do_nonstatic_fields(cl);
  }
  fieldDescriptor fd;
  int length = java_fields_count();
  // In DebugInfo nonstatic fields are sorted by offset.
  int* fields_sorted = NEW_C_HEAP_ARRAY(int, 2*(length+1), mtClass);
  int j = 0;
  for (int i = 0; i < length; i += 1) {
    fd.reinitialize(this, i);
    if (!fd.is_static()) {
      fields_sorted[j + 0] = fd.offset();
      fields_sorted[j + 1] = i;
      j += 2;
    }
  }
  if (j > 0) {
    length = j;
    // _sort_Fn is defined in growableArray.hpp.
    qsort(fields_sorted, length/2, 2*sizeof(int), (_sort_Fn)compare_fields_by_offset);
    for (int i = 0; i < length; i += 2) {
      fd.reinitialize(this, fields_sorted[i + 1]);
      assert(!fd.is_static() && fd.offset() == fields_sorted[i], "only nonstatic fields");
      cl->do_field(&fd);
    }
  }
  FREE_C_HEAP_ARRAY(int, fields_sorted);
}