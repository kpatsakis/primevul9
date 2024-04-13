static void trace_method_resolution(const char* prefix,
                                    Klass* klass,
                                    Klass* resolved_klass,
                                    Method* method,
                                    bool logitables,
                                    int index = -1) {
#ifndef PRODUCT
  ResourceMark rm;
  Log(itables) logi;
  LogStream lsi(logi.trace());
  Log(vtables) logv;
  LogStream lsv(logv.trace());
  outputStream* st;
  if (logitables) {
    st = &lsi;
  } else {
    st = &lsv;
  }
  st->print("%s%s, compile-time-class:%s, method:%s, method_holder:%s, access_flags: ",
            prefix,
            (klass == NULL ? "<NULL>" : klass->internal_name()),
            (resolved_klass == NULL ? "<NULL>" : resolved_klass->internal_name()),
            Method::name_and_sig_as_C_string(resolved_klass,
                                             method->name(),
                                             method->signature()),
            method->method_holder()->internal_name());
  method->print_linkage_flags(st);
  if (index != -1) {
    st->print("vtable_index:%d", index);
  }
  st->cr();
#endif // PRODUCT
}