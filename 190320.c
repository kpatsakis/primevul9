string t_cpp_generator::local_reflection_name(const char* prefix, t_type* ttype, bool external) {
  ttype = get_true_type(ttype);

  // We have to use the program name as part of the identifier because
  // if two thrift "programs" are compiled into one actual program
  // you would get a symbol collision if they both defined list<i32>.
  // trlo = Thrift Reflection LOcal.
  string prog;
  string name;
  string nspace;

  // TODO(dreiss): Would it be better to pregenerate the base types
  //               and put them in Thrift.{h,cpp} ?

  if (ttype->is_base_type()) {
    prog = program_->get_name();
    name = ttype->get_ascii_fingerprint();
  } else if (ttype->is_enum()) {
    assert(ttype->get_program() != NULL);
    prog = ttype->get_program()->get_name();
    name = ttype->get_ascii_fingerprint();
  } else if (ttype->is_container()) {
    prog = program_->get_name();
    name = ttype->get_ascii_fingerprint();
  } else {
    assert(ttype->is_struct() || ttype->is_xception());
    assert(ttype->get_program() != NULL);
    prog = ttype->get_program()->get_name();
    name = ttype->get_ascii_fingerprint();
  }

  if (external && ttype->get_program() != NULL && ttype->get_program() != program_) {
    nspace = namespace_prefix(ttype->get_program()->get_namespace("cpp"));
  }

  return nspace + "trlo_" + prefix + "_" + prog + "_" + name;
}