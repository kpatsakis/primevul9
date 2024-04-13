void t_cpp_generator::generate_enum_constant_list(std::ofstream& f,
                                                  const vector<t_enum_value*>& constants,
                                                  const char* prefix,
                                                  const char* suffix,
                                                  bool include_values) {
  f << " {" << endl;
  indent_up();

  vector<t_enum_value*>::const_iterator c_iter;
  bool first = true;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    if (first) {
      first = false;
    } else {
      f << "," << endl;
    }
    indent(f) << prefix << (*c_iter)->get_name() << suffix;
    if (include_values) {
      f << " = " << (*c_iter)->get_value();
    }
  }

  f << endl;
  indent_down();
  indent(f) << "};" << endl;
}