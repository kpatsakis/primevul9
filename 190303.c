void ProcessorGenerator::generate_dispatch_call(bool template_protocol) {
  string protocol = "::apache::thrift::protocol::TProtocol";
  string function_suffix;
  if (template_protocol) {
    protocol = "Protocol_";
    // We call the generic version dispatchCall(), and the specialized
    // version dispatchCallTemplated().  We can't call them both
    // dispatchCall(), since this will cause the compiler to issue a warning if
    // a service that doesn't use templates inherits from a service that does
    // use templates: the compiler complains that the subclass only implements
    // the generic version of dispatchCall(), and hides the templated version.
    // Using different names for the two functions prevents this.
    function_suffix = "Templated";
  }

  f_out_ << template_header_ << ret_type_ << class_name_ << template_suffix_ << "::dispatchCall"
         << function_suffix << "(" << finish_cob_ << protocol << "* iprot, " << protocol
         << "* oprot, "
         << "const std::string& fname, int32_t seqid" << call_context_ << ") {" << endl;
  indent_up();

  // HOT: member function pointer map
  f_out_ << indent() << typename_str_ << "ProcessMap::iterator pfn;" << endl << indent()
         << "pfn = processMap_.find(fname);" << endl << indent()
         << "if (pfn == processMap_.end()) {" << endl;
  if (extends_.empty()) {
    f_out_ << indent() << "  iprot->skip(::apache::thrift::protocol::T_STRUCT);" << endl << indent()
           << "  iprot->readMessageEnd();" << endl << indent()
           << "  iprot->getTransport()->readEnd();" << endl << indent()
           << "  ::apache::thrift::TApplicationException "
              "x(::apache::thrift::TApplicationException::UNKNOWN_METHOD, \"Invalid method name: "
              "'\"+fname+\"'\");" << endl << indent()
           << "  oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);"
           << endl << indent() << "  x.write(oprot);" << endl << indent()
           << "  oprot->writeMessageEnd();" << endl << indent()
           << "  oprot->getTransport()->writeEnd();" << endl << indent()
           << "  oprot->getTransport()->flush();" << endl << indent()
           << (style_ == "Cob" ? "  return cob(true);" : "  return true;") << endl;
  } else {
    f_out_ << indent() << "  return " << extends_ << "::dispatchCall("
           << (style_ == "Cob" ? "cob, " : "") << "iprot, oprot, fname, seqid" << call_context_arg_
           << ");" << endl;
  }
  f_out_ << indent() << "}" << endl;
  if (template_protocol) {
    f_out_ << indent() << "(this->*(pfn->second.specialized))";
  } else {
    if (generator_->gen_templates_only_) {
      // TODO: This is a null pointer, so nothing good will come from calling
      // it.  Throw an exception instead.
      f_out_ << indent() << "(this->*(pfn->second.generic))";
    } else if (generator_->gen_templates_) {
      f_out_ << indent() << "(this->*(pfn->second.generic))";
    } else {
      f_out_ << indent() << "(this->*(pfn->second))";
    }
  }
  f_out_ << "(" << cob_arg_ << "seqid, iprot, oprot" << call_context_arg_ << ");" << endl;

  // TODO(dreiss): return pfn ret?
  if (style_ == "Cob") {
    f_out_ << indent() << "return;" << endl;
  } else {
    f_out_ << indent() << "return true;" << endl;
  }

  indent_down();
  f_out_ << "}" << endl << endl;
}