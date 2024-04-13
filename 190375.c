void t_cpp_generator::generate_serialize_list_element(ofstream& out, t_list* tlist, string iter) {
  t_field efield(tlist->get_elem_type(), "(*" + iter + ")");
  generate_serialize_field(out, &efield, "");
}