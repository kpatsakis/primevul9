int  InstanceKlass::nof_implementors() const {
  InstanceKlass* ik = implementor();
  if (ik == NULL) {
    return 0;
  } else if (ik != this) {
    return 1;
  } else {
    return 2;
  }
}