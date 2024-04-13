int InstanceKlass::find_method_index(const Array<Method*>* methods,
                                     const Symbol* name,
                                     const Symbol* signature,
                                     OverpassLookupMode overpass_mode,
                                     StaticLookupMode static_mode,
                                     PrivateLookupMode private_mode) {
  const bool skipping_overpass = (overpass_mode == OverpassLookupMode::skip);
  const bool skipping_static = (static_mode == StaticLookupMode::skip);
  const bool skipping_private = (private_mode == PrivateLookupMode::skip);
  const int hit = quick_search(methods, name);
  if (hit != -1) {
    const Method* const m = methods->at(hit);

    // Do linear search to find matching signature.  First, quick check
    // for common case, ignoring overpasses if requested.
    if (method_matches(m, signature, skipping_overpass, skipping_static, skipping_private)) {
      return hit;
    }

    // search downwards through overloaded methods
    int i;
    for (i = hit - 1; i >= 0; --i) {
        const Method* const m = methods->at(i);
        assert(m->is_method(), "must be method");
        if (m->name() != name) {
          break;
        }
        if (method_matches(m, signature, skipping_overpass, skipping_static, skipping_private)) {
          return i;
        }
    }
    // search upwards
    for (i = hit + 1; i < methods->length(); ++i) {
        const Method* const m = methods->at(i);
        assert(m->is_method(), "must be method");
        if (m->name() != name) {
          break;
        }
        if (method_matches(m, signature, skipping_overpass, skipping_static, skipping_private)) {
          return i;
        }
    }
    // not found
#ifdef ASSERT
    const int index = (skipping_overpass || skipping_static || skipping_private) ? -1 :
      linear_search(methods, name, signature);
    assert(-1 == index, "binary search should have found entry %d", index);
#endif
  }
  return -1;
}