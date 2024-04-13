GrowableArray<Klass*>* InstanceKlass::compute_secondary_supers(int num_extra_slots,
                                                               Array<Klass*>* transitive_interfaces) {
  // The secondaries are the implemented interfaces.
  Array<Klass*>* interfaces = transitive_interfaces;
  int num_secondaries = num_extra_slots + interfaces->length();
  if (num_secondaries == 0) {
    // Must share this for correct bootstrapping!
    set_secondary_supers(Universe::the_empty_klass_array());
    return NULL;
  } else if (num_extra_slots == 0) {
    // The secondary super list is exactly the same as the transitive interfaces.
    // Redefine classes has to be careful not to delete this!
    set_secondary_supers(interfaces);
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