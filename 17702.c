void InstanceKlass::print_value_on(outputStream* st) const {
  assert(is_klass(), "must be klass");
  if (Verbose || WizardMode)  access_flags().print_on(st);
  name()->print_value_on(st);
}