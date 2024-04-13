void InstanceKlass::eager_initialize(Thread *thread) {
  if (!EagerInitialization) return;

  if (this->is_not_initialized()) {
    // abort if the the class has a class initializer
    if (this->class_initializer() != NULL) return;

    // abort if it is java.lang.Object (initialization is handled in genesis)
    Klass* super_klass = super();
    if (super_klass == NULL) return;

    // abort if the super class should be initialized
    if (!InstanceKlass::cast(super_klass)->is_initialized()) return;

    // call body to expose the this pointer
    eager_initialize_impl();
  }
}