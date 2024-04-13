void t_cpp_generator::generate_cpp_struct(t_struct* tstruct, bool is_exception) {
  generate_struct_declaration(f_types_, tstruct, is_exception, false, true, true, true);
  generate_struct_definition(f_types_impl_, f_types_impl_, tstruct);
  generate_struct_fingerprint(f_types_impl_, tstruct, true);
  generate_local_reflection(f_types_, tstruct, false);
  generate_local_reflection(f_types_impl_, tstruct, true);
  generate_local_reflection_pointer(f_types_impl_, tstruct);

  std::ofstream& out = (gen_templates_ ? f_types_tcc_ : f_types_impl_);
  generate_struct_reader(out, tstruct);
  generate_struct_writer(out, tstruct);
  generate_struct_swap(f_types_impl_, tstruct);
  generate_copy_constructor(f_types_impl_, tstruct, is_exception);
  if (gen_moveable_) {
    generate_move_constructor(f_types_impl_, tstruct, is_exception);
  }
  generate_assignment_operator(f_types_impl_, tstruct);
  if (gen_moveable_) {
    generate_move_assignment_operator(f_types_impl_, tstruct);
  }
  generate_struct_ostream_operator(f_types_impl_, tstruct);
  if (is_exception) {
    generate_exception_what_method(f_types_impl_, tstruct);
  }
}