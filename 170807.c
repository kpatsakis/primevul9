void InstanceKlass::call_class_initializer(TRAPS) {
  if (ReplayCompiles &&
      (ReplaySuppressInitializers == 1 ||
       (ReplaySuppressInitializers >= 2 && class_loader() != NULL))) {
    // Hide the existence of the initializer for the purpose of replaying the compile
    return;
  }

  methodHandle h_method(THREAD, class_initializer());
  assert(!is_initialized(), "we cannot initialize twice");
  LogTarget(Info, class, init) lt;
  if (lt.is_enabled()) {
    ResourceMark rm(THREAD);
    LogStream ls(lt);
    ls.print("%d Initializing ", call_class_initializer_counter++);
    name()->print_value_on(&ls);
    ls.print_cr("%s (" INTPTR_FORMAT ")", h_method() == NULL ? "(no method)" : "", p2i(this));
  }
  if (h_method() != NULL) {
    JavaCallArguments args; // No arguments
    JavaValue result(T_VOID);
    JavaCalls::call(&result, h_method, &args, CHECK); // Static call (no args)
  }
}