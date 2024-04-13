void t_cpp_generator::generate_consts(std::vector<t_const*> consts) {
  string f_consts_name = get_out_dir() + program_name_ + "_constants.h";
  ofstream f_consts;
  f_consts.open(f_consts_name.c_str());

  string f_consts_impl_name = get_out_dir() + program_name_ + "_constants.cpp";
  ofstream f_consts_impl;
  f_consts_impl.open(f_consts_impl_name.c_str());

  // Print header
  f_consts << autogen_comment();
  f_consts_impl << autogen_comment();

  // Start ifndef
  f_consts << "#ifndef " << program_name_ << "_CONSTANTS_H" << endl << "#define " << program_name_
           << "_CONSTANTS_H" << endl << endl << "#include \"" << get_include_prefix(*get_program())
           << program_name_ << "_types.h\"" << endl << endl << ns_open_ << endl << endl;

  f_consts_impl << "#include \"" << get_include_prefix(*get_program()) << program_name_
                << "_constants.h\"" << endl << endl << ns_open_ << endl << endl;

  f_consts << "class " << program_name_ << "Constants {" << endl << " public:" << endl << "  "
           << program_name_ << "Constants();" << endl << endl;
  indent_up();
  vector<t_const*>::iterator c_iter;
  for (c_iter = consts.begin(); c_iter != consts.end(); ++c_iter) {
    string name = (*c_iter)->get_name();
    t_type* type = (*c_iter)->get_type();
    f_consts << indent() << type_name(type) << " " << name << ";" << endl;
  }
  indent_down();
  f_consts << "};" << endl;

  f_consts_impl << "const " << program_name_ << "Constants g_" << program_name_ << "_constants;"
                << endl << endl << program_name_ << "Constants::" << program_name_
                << "Constants() {" << endl;
  indent_up();
  for (c_iter = consts.begin(); c_iter != consts.end(); ++c_iter) {
    print_const_value(f_consts_impl,
                      (*c_iter)->get_name(),
                      (*c_iter)->get_type(),
                      (*c_iter)->get_value());
  }
  indent_down();
  indent(f_consts_impl) << "}" << endl;

  f_consts << endl << "extern const " << program_name_ << "Constants g_" << program_name_
           << "_constants;" << endl << endl << ns_close_ << endl << endl << "#endif" << endl;
  f_consts.close();

  f_consts_impl << endl << ns_close_ << endl << endl;
}