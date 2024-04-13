void t_cpp_generator::generate_struct_result_writer(ofstream& out,
                                                    t_struct* tstruct,
                                                    bool pointers) {
  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  if (gen_templates_) {
    out << indent() << "template <class Protocol_>" << endl << indent() << "uint32_t "
        << tstruct->get_name() << "::write(Protocol_* oprot) const {" << endl;
  } else {
    indent(out) << "uint32_t " << tstruct->get_name()
                << "::write(::apache::thrift::protocol::TProtocol* oprot) const {" << endl;
  }
  indent_up();

  out << endl << indent() << "uint32_t xfer = 0;" << endl << endl;

  indent(out) << "xfer += oprot->writeStructBegin(\"" << name << "\");" << endl;

  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
      out << endl << indent() << "if ";
    } else {
      out << " else if ";
    }

    out << "(this->__isset." << (*f_iter)->get_name() << ") {" << endl;

    indent_up();

    // Write field header
    out << indent() << "xfer += oprot->writeFieldBegin("
        << "\"" << (*f_iter)->get_name() << "\", " << type_to_enum((*f_iter)->get_type()) << ", "
        << (*f_iter)->get_key() << ");" << endl;
    // Write field contents
    if (pointers) {
      generate_serialize_field(out, *f_iter, "(*(this->", "))");
    } else {
      generate_serialize_field(out, *f_iter, "this->");
    }
    // Write field closer
    indent(out) << "xfer += oprot->writeFieldEnd();" << endl;

    indent_down();
    indent(out) << "}";
  }

  // Write the struct map
  out << endl << indent() << "xfer += oprot->writeFieldStop();" << endl << indent()
      << "xfer += oprot->writeStructEnd();" << endl << indent() << "return xfer;" << endl;

  indent_down();
  indent(out) << "}" << endl << endl;
}