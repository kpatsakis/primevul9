instanceOop InstanceKlass::register_finalizer(instanceOop i, TRAPS) {
  if (TraceFinalizerRegistration) {
    tty->print("Registered ");
    i->print_value_on(tty);
    tty->print_cr(" (" INTPTR_FORMAT ") as finalizable", p2i(i));
  }
  instanceHandle h_i(THREAD, i);
  // Pass the handle as argument, JavaCalls::call expects oop as jobjects
  JavaValue result(T_VOID);
  JavaCallArguments args(h_i);
  methodHandle mh (THREAD, Universe::finalizer_register_method());
  JavaCalls::call(&result, mh, &args, CHECK_NULL);
  return h_i();
}