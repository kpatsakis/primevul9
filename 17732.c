static bool method_matches(const Method* m,
                           const Symbol* signature,
                           bool skipping_overpass,
                           bool skipping_static,
                           bool skipping_private) {
  return ((m->signature() == signature) &&
    (!skipping_overpass || !m->is_overpass()) &&
    (!skipping_static || !m->is_static()) &&
    (!skipping_private || !m->is_private()));
}