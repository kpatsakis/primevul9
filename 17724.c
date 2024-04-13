InstanceKlass::InstanceKlass(const ClassFileParser& parser, unsigned kind, KlassID id) :
  Klass(id),
  _static_field_size(parser.static_field_size()),
  _nonstatic_oop_map_size(nonstatic_oop_map_size(parser.total_oop_map_count())),
  _itable_len(parser.itable_size()),
  _init_thread(NULL),
  _init_state(allocated),
  _reference_type(parser.reference_type()),
  _nest_members(NULL),
  _nest_host_index(0),
  _nest_host(NULL) {
    set_vtable_length(parser.vtable_size());
    set_kind(kind);
    set_access_flags(parser.access_flags());
    set_is_anonymous(parser.is_anonymous());
    set_layout_helper(Klass::instance_layout_helper(parser.layout_size(),
                                                    false));

    assert(NULL == _methods, "underlying memory not zeroed?");
    assert(is_instance_klass(), "is layout incorrect?");
    assert(size_helper() == parser.layout_size(), "incorrect size_helper?");
}