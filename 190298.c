void t_cpp_generator::generate_service_interface(t_service* tservice, string style) {

  string service_if_name = service_name_ + style + "If";
  if (style == "CobCl") {
    // Forward declare the client.
    string client_name = service_name_ + "CobClient";
    if (gen_templates_) {
      client_name += "T";
      service_if_name += "T";
      indent(f_header_) << "template <class Protocol_>" << endl;
    }
    indent(f_header_) << "class " << client_name << ";" << endl << endl;
  }

  string extends = "";
  if (tservice->get_extends() != NULL) {
    extends = " : virtual public " + type_name(tservice->get_extends()) + style + "If";
    if (style == "CobCl" && gen_templates_) {
      // TODO(simpkins): If gen_templates_ is enabled, we currently assume all
      // parent services were also generated with templates enabled.
      extends += "T<Protocol_>";
    }
  }

  if (style == "CobCl" && gen_templates_) {
    f_header_ << "template <class Protocol_>" << endl;
  }
  f_header_ << "class " << service_if_name << extends << " {" << endl << " public:" << endl;
  indent_up();
  f_header_ << indent() << "virtual ~" << service_if_name << "() {}" << endl;

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    if ((*f_iter)->has_doc())
      f_header_ << endl;
    generate_java_doc(f_header_, *f_iter);
    f_header_ << indent() << "virtual " << function_signature(*f_iter, style) << " = 0;" << endl;
  }
  indent_down();
  f_header_ << "};" << endl << endl;

  if (style == "CobCl" && gen_templates_) {
    // generate a backwards-compatible typedef for clients that do not
    // know about the new template-style code
    f_header_ << "typedef " << service_if_name << "< ::apache::thrift::protocol::TProtocol> "
              << service_name_ << style << "If;" << endl << endl;
  }
}