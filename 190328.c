void t_cpp_generator::generate_service_null(t_service* tservice, string style) {
  string extends = "";
  if (tservice->get_extends() != NULL) {
    extends = " , virtual public " + type_name(tservice->get_extends()) + style + "Null";
  }
  f_header_ << "class " << service_name_ << style << "Null : virtual public " << service_name_
            << style << "If" << extends << " {" << endl << " public:" << endl;
  indent_up();
  f_header_ << indent() << "virtual ~" << service_name_ << style << "Null() {}" << endl;
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_header_ << indent() << function_signature(*f_iter, style, "", false) << " {" << endl;
    indent_up();

    t_type* returntype = (*f_iter)->get_returntype();
    t_field returnfield(returntype, "_return");

    if (style == "") {
      if (returntype->is_void() || is_complex_type(returntype)) {
        f_header_ << indent() << "return;" << endl;
      } else {
        f_header_ << indent() << declare_field(&returnfield, true) << endl << indent()
                  << "return _return;" << endl;
      }
    } else if (style == "CobSv") {
      if (returntype->is_void()) {
        f_header_ << indent() << "return cob();" << endl;
      } else {
        t_field returnfield(returntype, "_return");
        f_header_ << indent() << declare_field(&returnfield, true) << endl << indent()
                  << "return cob(_return);" << endl;
      }

    } else {
      throw "UNKNOWN STYLE";
    }

    indent_down();
    f_header_ << indent() << "}" << endl;
  }
  indent_down();
  f_header_ << "};" << endl << endl;
}