void t_cpp_generator::generate_serialize_container(ofstream& out, t_type* ttype, string prefix) {
  scope_up(out);

  if (ttype->is_map()) {
    indent(out) << "xfer += oprot->writeMapBegin(" << type_to_enum(((t_map*)ttype)->get_key_type())
                << ", " << type_to_enum(((t_map*)ttype)->get_val_type()) << ", "
                << "static_cast<uint32_t>(" << prefix << ".size()));" << endl;
  } else if (ttype->is_set()) {
    indent(out) << "xfer += oprot->writeSetBegin(" << type_to_enum(((t_set*)ttype)->get_elem_type())
                << ", "
                << "static_cast<uint32_t>(" << prefix << ".size()));" << endl;
  } else if (ttype->is_list()) {
    indent(out) << "xfer += oprot->writeListBegin("
                << type_to_enum(((t_list*)ttype)->get_elem_type()) << ", "
                << "static_cast<uint32_t>(" << prefix << ".size()));" << endl;
  }

  string iter = tmp("_iter");
  out << indent() << type_name(ttype) << "::const_iterator " << iter << ";" << endl << indent()
      << "for (" << iter << " = " << prefix << ".begin(); " << iter << " != " << prefix
      << ".end(); ++" << iter << ")" << endl;
  scope_up(out);
  if (ttype->is_map()) {
    generate_serialize_map_element(out, (t_map*)ttype, iter);
  } else if (ttype->is_set()) {
    generate_serialize_set_element(out, (t_set*)ttype, iter);
  } else if (ttype->is_list()) {
    generate_serialize_list_element(out, (t_list*)ttype, iter);
  }
  scope_down(out);

  if (ttype->is_map()) {
    indent(out) << "xfer += oprot->writeMapEnd();" << endl;
  } else if (ttype->is_set()) {
    indent(out) << "xfer += oprot->writeSetEnd();" << endl;
  } else if (ttype->is_list()) {
    indent(out) << "xfer += oprot->writeListEnd();" << endl;
  }

  scope_down(out);
}