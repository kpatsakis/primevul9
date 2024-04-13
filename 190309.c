void t_cpp_generator::generate_constructor_helper(ofstream& out,
                                                  t_struct* tstruct,
                                                  bool is_exception,
                                                  bool is_move) {

  std::string tmp_name = tmp("other");

  indent(out) << tstruct->get_name() << "::" << tstruct->get_name();

  if (is_move) {
    out << "( " << tstruct->get_name() << "&& ";
  } else {
    out << "(const " << tstruct->get_name() << "& ";
  }
  out << tmp_name << ") ";
  if (is_exception)
    out << ": TException() ";
  out << "{" << endl;
  indent_up();

  const vector<t_field*>& members = tstruct->get_members();

  // eliminate compiler unused warning
  if (members.empty())
    indent(out) << "(void) " << tmp_name << ";" << endl;

  vector<t_field*>::const_iterator f_iter;
  bool has_nonrequired_fields = false;
  for (f_iter = members.begin(); f_iter != members.end(); ++f_iter) {
    if ((*f_iter)->get_req() != t_field::T_REQUIRED)
      has_nonrequired_fields = true;
    indent(out) << (*f_iter)->get_name() << " = "
                << maybeMove(tmp_name + "." + (*f_iter)->get_name(), is_move) << ";" << endl;
  }

  if (has_nonrequired_fields) {
    indent(out) << "__isset = " << maybeMove(tmp_name + ".__isset", is_move) << ";" << endl;
  }

  indent_down();
  indent(out) << "}" << endl;
}