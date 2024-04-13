int InstanceKlass::mark_dependent_nmethods(KlassDepChange& changes) {
  return dependencies().mark_dependent_nmethods(changes);
}