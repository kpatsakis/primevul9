void t_cpp_generator::generate_struct_ostream_operator(std::ofstream& out, t_struct* tstruct) {
  out << indent();
  generate_struct_ostream_operator_decl(out, tstruct);
  out << " {" << endl;

  indent_up();

  out << indent() << "using apache::thrift::to_string;" << endl;

  // eliminate compiler unused warning
  const vector<t_field*>& fields = tstruct->get_members();
  if (fields.empty())
    out << indent() << "(void) obj;" << endl;

  out << indent() << "out << \"" << tstruct->get_name() << "(\";" << endl;

  struct_ostream_operator_generator::generate_fields(out, fields, indent());

  out << indent() << "out << \")\";" << endl << indent() << "return out;" << endl;

  indent_down();
  out << "}" << endl << endl;
}