GrowableArray<Klass*>* InstanceKlass::compute_secondary_supers(int num_extra_slots,
                                                               Array<InstanceKlass*>* transitive_interfaces) {
  // The secondaries are the implemented interfaces.
  Array<InstanceKlass*>* interfaces = transitive_interfaces;
  int num_secondaries = num_extra_slots + interfaces->length();
  if (num_secondaries == 0) {
    // Must share this for correct bootstrapping!
    set_secondary_supers(Universe::the_empty_klass_array());
    return NULL;
  } else if (num_extra_slots == 0) {
    // The secondary super list is exactly the same as the transitive interfaces, so
    // let's use it instead of making a copy.
    // Redefine classes has to be careful not to delete this!
    // We need the cast because Array<Klass*> is NOT a supertype of Array<InstanceKlass*>,
    // (but it's safe to do here because we won't write into _secondary_supers from this point on).
    set_secondary_supers((Array<Klass*>*)(address)interfaces);
    return NULL;
  } else {
    // Copy transitive interfaces to a temporary growable array to be constructed
    // into the secondary super list with extra slots.
    GrowableArray<Klass*>* secondaries = new GrowableArray<Klass*>(interfaces->length());
    for (int i = 0; i < interfaces->length(); i++) {
      secondaries->push(interfaces->at(i));
    }
    return secondaries;
  }
}