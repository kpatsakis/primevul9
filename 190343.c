void t_cpp_generator::generate_deserialize_container(ofstream& out, t_type* ttype, string prefix) {
  scope_up(out);

  string size = tmp("_size");
  string ktype = tmp("_ktype");
  string vtype = tmp("_vtype");
  string etype = tmp("_etype");

  t_container* tcontainer = (t_container*)ttype;
  bool use_push = tcontainer->has_cpp_name();

  indent(out) << prefix << ".clear();" << endl << indent() << "uint32_t " << size << ";" << endl;

  // Declare variables, read header
  if (ttype->is_map()) {
    out << indent() << "::apache::thrift::protocol::TType " << ktype << ";" << endl << indent()
        << "::apache::thrift::protocol::TType " << vtype << ";" << endl << indent()
        << "xfer += iprot->readMapBegin(" << ktype << ", " << vtype << ", " << size << ");" << endl;
  } else if (ttype->is_set()) {
    out << indent() << "::apache::thrift::protocol::TType " << etype << ";" << endl << indent()
        << "xfer += iprot->readSetBegin(" << etype << ", " << size << ");" << endl;
  } else if (ttype->is_list()) {
    out << indent() << "::apache::thrift::protocol::TType " << etype << ";" << endl << indent()
        << "xfer += iprot->readListBegin(" << etype << ", " << size << ");" << endl;
    if (!use_push) {
      indent(out) << prefix << ".resize(" << size << ");" << endl;
    }
  }

  // For loop iterates over elements
  string i = tmp("_i");
  out << indent() << "uint32_t " << i << ";" << endl << indent() << "for (" << i << " = 0; " << i
      << " < " << size << "; ++" << i << ")" << endl;

  scope_up(out);

  if (ttype->is_map()) {
    generate_deserialize_map_element(out, (t_map*)ttype, prefix);
  } else if (ttype->is_set()) {
    generate_deserialize_set_element(out, (t_set*)ttype, prefix);
  } else if (ttype->is_list()) {
    generate_deserialize_list_element(out, (t_list*)ttype, prefix, use_push, i);
  }

  scope_down(out);

  // Read container end
  if (ttype->is_map()) {
    indent(out) << "xfer += iprot->readMapEnd();" << endl;
  } else if (ttype->is_set()) {
    indent(out) << "xfer += iprot->readSetEnd();" << endl;
  } else if (ttype->is_list()) {
    indent(out) << "xfer += iprot->readListEnd();" << endl;
  }

  scope_down(out);
}