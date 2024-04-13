Method* InstanceKlass::method_at_itable(Klass* holder, int index, TRAPS) {
  itableOffsetEntry* ioe = (itableOffsetEntry*)start_of_itable();
  int method_table_offset_in_words = ioe->offset()/wordSize;
  int nof_interfaces = (method_table_offset_in_words - itable_offset_in_words())
                       / itableOffsetEntry::size();

  for (int cnt = 0 ; ; cnt ++, ioe ++) {
    // If the interface isn't implemented by the receiver class,
    // the VM should throw IncompatibleClassChangeError.
    if (cnt >= nof_interfaces) {
      ResourceMark rm(THREAD);
      stringStream ss;
      bool same_module = (module() == holder->module());
      ss.print("Receiver class %s does not implement "
               "the interface %s defining the method to be called "
               "(%s%s%s)",
               external_name(), holder->external_name(),
               (same_module) ? joint_in_module_of_loader(holder) : class_in_module_of_loader(),
               (same_module) ? "" : "; ",
               (same_module) ? "" : holder->class_in_module_of_loader());
      THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
    }

    Klass* ik = ioe->interface_klass();
    if (ik == holder) break;
  }

  itableMethodEntry* ime = ioe->first_method_entry(this);
  Method* m = ime[index].method();
  if (m == NULL) {
    THROW_NULL(vmSymbols::java_lang_AbstractMethodError());
  }
  return m;
}