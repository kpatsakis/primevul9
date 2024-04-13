void ProcessorGenerator::generate_class_definition() {
  // Generate the dispatch methods
  vector<t_function*> functions = service_->get_functions();
  vector<t_function*>::iterator f_iter;

  string parent_class;
  if (service_->get_extends() != NULL) {
    parent_class = extends_;
  } else {
    if (style_ == "Cob") {
      parent_class = "::apache::thrift::async::TAsyncDispatchProcessor";
    } else {
      parent_class = "::apache::thrift::TDispatchProcessor";
    }

    if (generator_->gen_templates_) {
      parent_class += "T<Protocol_>";
    }
  }

  // Generate the header portion
  f_header_ << template_header_ << "class " << class_name_ << " : public " << parent_class << " {"
            << endl;

  // Protected data members
  f_header_ << " protected:" << endl;
  indent_up();
  f_header_ << indent() << "boost::shared_ptr<" << if_name_ << "> iface_;" << endl;
  f_header_ << indent() << "virtual " << ret_type_ << "dispatchCall(" << finish_cob_
            << "::apache::thrift::protocol::TProtocol* iprot, "
            << "::apache::thrift::protocol::TProtocol* oprot, "
            << "const std::string& fname, int32_t seqid" << call_context_ << ");" << endl;
  if (generator_->gen_templates_) {
    f_header_ << indent() << "virtual " << ret_type_ << "dispatchCallTemplated(" << finish_cob_
              << "Protocol_* iprot, Protocol_* oprot, "
              << "const std::string& fname, int32_t seqid" << call_context_ << ");" << endl;
  }
  indent_down();

  // Process function declarations
  f_header_ << " private:" << endl;
  indent_up();

  // Declare processMap_
  f_header_ << indent() << "typedef  void (" << class_name_ << "::*"
            << "ProcessFunction)(" << finish_cob_decl_ << "int32_t, "
            << "::apache::thrift::protocol::TProtocol*, "
            << "::apache::thrift::protocol::TProtocol*" << call_context_decl_ << ");" << endl;
  if (generator_->gen_templates_) {
    f_header_ << indent() << "typedef void (" << class_name_ << "::*"
              << "SpecializedProcessFunction)(" << finish_cob_decl_ << "int32_t, "
              << "Protocol_*, Protocol_*" << call_context_decl_ << ");" << endl << indent()
              << "struct ProcessFunctions {" << endl << indent() << "  ProcessFunction generic;"
              << endl << indent() << "  SpecializedProcessFunction specialized;" << endl << indent()
              << "  ProcessFunctions(ProcessFunction g, "
              << "SpecializedProcessFunction s) :" << endl << indent() << "    generic(g)," << endl
              << indent() << "    specialized(s) {}" << endl << indent()
              << "  ProcessFunctions() : generic(NULL), specialized(NULL) "
              << "{}" << endl << indent() << "};" << endl << indent()
              << "typedef std::map<std::string, ProcessFunctions> "
              << "ProcessMap;" << endl;
  } else {
    f_header_ << indent() << "typedef std::map<std::string, ProcessFunction> "
              << "ProcessMap;" << endl;
  }
  f_header_ << indent() << "ProcessMap processMap_;" << endl;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    indent(f_header_) << "void process_" << (*f_iter)->get_name() << "(" << finish_cob_
                      << "int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, "
                         "::apache::thrift::protocol::TProtocol* oprot" << call_context_ << ");"
                      << endl;
    if (generator_->gen_templates_) {
      indent(f_header_) << "void process_" << (*f_iter)->get_name() << "(" << finish_cob_
                        << "int32_t seqid, Protocol_* iprot, Protocol_* oprot" << call_context_
                        << ");" << endl;
    }
    if (style_ == "Cob") {
      // XXX Factor this out, even if it is a pain.
      string ret_arg = ((*f_iter)->get_returntype()->is_void()
                            ? ""
                            : ", const " + type_name((*f_iter)->get_returntype()) + "& _return");
      f_header_ << indent() << "void return_" << (*f_iter)->get_name()
                << "(tcxx::function<void(bool ok)> cob, int32_t seqid, "
                << "::apache::thrift::protocol::TProtocol* oprot, "
                << "void* ctx" << ret_arg << ");" << endl;
      if (generator_->gen_templates_) {
        f_header_ << indent() << "void return_" << (*f_iter)->get_name()
                  << "(tcxx::function<void(bool ok)> cob, int32_t seqid, "
                  << "Protocol_* oprot, void* ctx" << ret_arg << ");" << endl;
      }
      // XXX Don't declare throw if it doesn't exist
      f_header_ << indent() << "void throw_" << (*f_iter)->get_name()
                << "(tcxx::function<void(bool ok)> cob, int32_t seqid, "
                << "::apache::thrift::protocol::TProtocol* oprot, void* ctx, "
                << "::apache::thrift::TDelayedException* _throw);" << endl;
      if (generator_->gen_templates_) {
        f_header_ << indent() << "void throw_" << (*f_iter)->get_name()
                  << "(tcxx::function<void(bool ok)> cob, int32_t seqid, "
                  << "Protocol_* oprot, void* ctx, "
                  << "::apache::thrift::TDelayedException* _throw);" << endl;
      }
    }
  }

  f_header_ << " public:" << endl << indent() << class_name_ << "(boost::shared_ptr<" << if_name_
            << "> iface) :" << endl;
  if (!extends_.empty()) {
    f_header_ << indent() << "  " << extends_ << "(iface)," << endl;
  }
  f_header_ << indent() << "  iface_(iface) {" << endl;
  indent_up();

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_header_ << indent() << "processMap_[\"" << (*f_iter)->get_name() << "\"] = ";
    if (generator_->gen_templates_) {
      f_header_ << "ProcessFunctions(" << endl;
      if (generator_->gen_templates_only_) {
        indent(f_header_) << "  NULL," << endl;
      } else {
        indent(f_header_) << "  &" << class_name_ << "::process_" << (*f_iter)->get_name() << ","
                          << endl;
      }
      indent(f_header_) << "  &" << class_name_ << "::process_" << (*f_iter)->get_name() << ")";
    } else {
      f_header_ << "&" << class_name_ << "::process_" << (*f_iter)->get_name();
    }
    f_header_ << ";" << endl;
  }

  indent_down();
  f_header_ << indent() << "}" << endl << endl << indent() << "virtual ~" << class_name_ << "() {}"
            << endl;
  indent_down();
  f_header_ << "};" << endl << endl;

  if (generator_->gen_templates_) {
    // Generate a backwards compatible typedef, for callers who don't know
    // about the new template-style code.
    //
    // We can't use TProtocol as the template parameter, since ProcessorT
    // provides overloaded versions of most methods, one of which accepts
    // TProtocol pointers, and one which accepts Protocol_ pointers.  This
    // results in a compile error if instantiated with Protocol_ == TProtocol.
    // Therefore, we define TDummyProtocol solely so we can use it as the
    // template parameter here.
    f_header_ << "typedef " << class_name_ << "< ::apache::thrift::protocol::TDummyProtocol > "
              << service_name_ << pstyle_ << "Processor;" << endl << endl;
  }
}