void t_cpp_generator::generate_struct_swap(ofstream& out, t_struct* tstruct) {
  out << indent() << "void swap(" << tstruct->get_name() << " &a, " << tstruct->get_name()
      << " &b) {" << endl;
  indent_up();

  // Let argument-dependent name lookup find the correct swap() function to
  // use based on the argument types.  If none is found in the arguments'
  // namespaces, fall back to ::std::swap().
  out << indent() << "using ::std::swap;" << endl;

  bool has_nonrequired_fields = false;
  const vector<t_field*>& fields = tstruct->get_members();
  for (vector<t_field*>::const_iterator f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* tfield = *f_iter;

    if (tfield->get_req() != t_field::T_REQUIRED) {
      has_nonrequired_fields = true;
    }

    out << indent() << "swap(a." << tfield->get_name() << ", b." << tfield->get_name() << ");"
        << endl;
  }

  if (has_nonrequired_fields) {
    out << indent() << "swap(a.__isset, b.__isset);" << endl;
  }

  // handle empty structs
  if (fields.size() == 0) {
    out << indent() << "(void) a;" << endl;
    out << indent() << "(void) b;" << endl;
  }

  scope_down(out);
  out << endl;
}