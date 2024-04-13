void t_cpp_generator::generate_serialize_set_element(ofstream& out, t_set* tset, string iter) {
  t_field efield(tset->get_elem_type(), "(*" + iter + ")");
  generate_serialize_field(out, &efield, "");
}