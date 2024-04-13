void t_cpp_generator::generate_service_multiface(t_service* tservice) {
  // Generate the dispatch methods
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  string extends = "";
  string extends_multiface = "";
  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    extends_multiface = ", public " + extends + "Multiface";
  }

  string list_type = string("std::vector<boost::shared_ptr<") + service_name_ + "If> >";

  // Generate the header portion
  f_header_ << "class " << service_name_ << "Multiface : "
            << "virtual public " << service_name_ << "If" << extends_multiface << " {" << endl
            << " public:" << endl;
  indent_up();
  f_header_ << indent() << service_name_ << "Multiface(" << list_type
            << "& ifaces) : ifaces_(ifaces) {" << endl;
  if (!extends.empty()) {
    f_header_ << indent()
              << "  std::vector<boost::shared_ptr<" + service_name_ + "If> >::iterator iter;"
              << endl << indent() << "  for (iter = ifaces.begin(); iter != ifaces.end(); ++iter) {"
              << endl << indent() << "    " << extends << "Multiface::add(*iter);" << endl
              << indent() << "  }" << endl;
  }
  f_header_ << indent() << "}" << endl << indent() << "virtual ~" << service_name_
            << "Multiface() {}" << endl;
  indent_down();

  // Protected data members
  f_header_ << " protected:" << endl;
  indent_up();
  f_header_ << indent() << list_type << " ifaces_;" << endl << indent() << service_name_
            << "Multiface() {}" << endl << indent() << "void add(boost::shared_ptr<"
            << service_name_ << "If> iface) {" << endl;
  if (!extends.empty()) {
    f_header_ << indent() << "  " << extends << "Multiface::add(iface);" << endl;
  }
  f_header_ << indent() << "  ifaces_.push_back(iface);" << endl << indent() << "}" << endl;
  indent_down();

  f_header_ << indent() << " public:" << endl;
  indent_up();

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* arglist = (*f_iter)->get_arglist();
    const vector<t_field*>& args = arglist->get_members();
    vector<t_field*>::const_iterator a_iter;

    string call = string("ifaces_[i]->") + (*f_iter)->get_name() + "(";
    bool first = true;
    if (is_complex_type((*f_iter)->get_returntype())) {
      call += "_return";
      first = false;
    }
    for (a_iter = args.begin(); a_iter != args.end(); ++a_iter) {
      if (first) {
        first = false;
      } else {
        call += ", ";
      }
      call += (*a_iter)->get_name();
    }
    call += ")";

    f_header_ << indent() << function_signature(*f_iter, "") << " {" << endl;
    indent_up();
    f_header_ << indent() << "size_t sz = ifaces_.size();" << endl << indent() << "size_t i = 0;"
              << endl << indent() << "for (; i < (sz - 1); ++i) {" << endl;
    indent_up();
    f_header_ << indent() << call << ";" << endl;
    indent_down();
    f_header_ << indent() << "}" << endl;

    if (!(*f_iter)->get_returntype()->is_void()) {
      if (is_complex_type((*f_iter)->get_returntype())) {
        f_header_ << indent() << call << ";" << endl << indent() << "return;" << endl;
      } else {
        f_header_ << indent() << "return " << call << ";" << endl;
      }
    } else {
      f_header_ << indent() << call << ";" << endl;
    }

    indent_down();
    f_header_ << indent() << "}" << endl << endl;
  }

  indent_down();
  f_header_ << indent() << "};" << endl << endl;
}