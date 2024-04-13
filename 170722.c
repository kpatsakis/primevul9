Method* InstanceKlass::method_at_itable_or_null(InstanceKlass* holder, int index, bool& implements_interface) {
  klassItable itable(this);
  for (int i = 0; i < itable.size_offset_table(); i++) {
    itableOffsetEntry* offset_entry = itable.offset_entry(i);
    if (offset_entry->interface_klass() == holder) {
      implements_interface = true;
      itableMethodEntry* ime = offset_entry->first_method_entry(this);
      Method* m = ime[index].method();
      return m;
    }
  }
  implements_interface = false;
  return NULL; // offset entry not found
}