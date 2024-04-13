string t_cpp_generator::argument_list(t_struct* tstruct, bool name_params, bool start_comma) {
  string result = "";

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  bool first = !start_comma;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      result += ", ";
    }
    result += type_name((*f_iter)->get_type(), false, true) + " "
              + (name_params ? (*f_iter)->get_name() : "/* " + (*f_iter)->get_name() + " */");
  }
  return result;
}