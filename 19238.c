static void print_nest_host_error_on(stringStream* ss, Klass* ref_klass, Klass* sel_klass) {
  assert(ref_klass->is_instance_klass(), "must be");
  assert(sel_klass->is_instance_klass(), "must be");
  InstanceKlass* ref_ik = InstanceKlass::cast(ref_klass);
  InstanceKlass* sel_ik = InstanceKlass::cast(sel_klass);
  const char* nest_host_error_1 = ref_ik->nest_host_error();
  const char* nest_host_error_2 = sel_ik->nest_host_error();
  if (nest_host_error_1 != NULL || nest_host_error_2 != NULL) {
    ss->print(", (%s%s%s)",
              (nest_host_error_1 != NULL) ? nest_host_error_1 : "",
              (nest_host_error_1 != NULL && nest_host_error_2 != NULL) ? ", " : "",
              (nest_host_error_2 != NULL) ? nest_host_error_2 : "");
  }
}