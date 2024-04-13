void t_cpp_generator::generate_service_client(t_service* tservice, string style) {
  string ifstyle;
  if (style == "Cob") {
    ifstyle = "CobCl";
  }

  std::ofstream& out = (gen_templates_ ? f_service_tcc_ : f_service_);
  string template_header, template_suffix, short_suffix, protocol_type, _this;
  string const prot_factory_type = "::apache::thrift::protocol::TProtocolFactory";
  if (gen_templates_) {
    template_header = "template <class Protocol_>\n";
    short_suffix = "T";
    template_suffix = "T<Protocol_>";
    protocol_type = "Protocol_";
    _this = "this->";
  } else {
    protocol_type = "::apache::thrift::protocol::TProtocol";
  }
  string prot_ptr = "boost::shared_ptr< " + protocol_type + ">";
  string client_suffix = "Client" + template_suffix;
  string if_suffix = "If";
  if (style == "Cob") {
    if_suffix += template_suffix;
  }

  string extends = "";
  string extends_client = "";
  if (tservice->get_extends() != NULL) {
    // TODO(simpkins): If gen_templates_ is enabled, we currently assume all
    // parent services were also generated with templates enabled.
    extends = type_name(tservice->get_extends());
    extends_client = ", public " + extends + style + client_suffix;
  }

  // Generate the header portion
  f_header_ << template_header << "class " << service_name_ << style << "Client" << short_suffix
            << " : "
            << "virtual public " << service_name_ << ifstyle << if_suffix << extends_client << " {"
            << endl << " public:" << endl;

  indent_up();
  if (style != "Cob") {
    f_header_ << indent() << service_name_ << style << "Client" << short_suffix << "(" << prot_ptr
              << " prot) ";

    if (extends.empty()) {
      f_header_ << "{" << endl;
      f_header_ << indent() << "  setProtocol" << short_suffix << "(prot);" << endl << indent()
                << "}" << endl;
    } else {
      f_header_ << ":" << endl;
      f_header_ << indent() << "  " << extends << style << client_suffix << "(prot, prot) {}"
                << endl;
    }

    f_header_ << indent() << service_name_ << style << "Client" << short_suffix << "(" << prot_ptr
              << " iprot, " << prot_ptr << " oprot) ";
    if (extends.empty()) {
      f_header_ << "{" << endl;
      f_header_ << indent() << "  setProtocol" << short_suffix << "(iprot,oprot);" << endl
                << indent() << "}" << endl;
    } else {
      f_header_ << ":" << indent() << "  " << extends << style << client_suffix
                << "(iprot, oprot) {}" << endl;
    }

    // create the setProtocol methods
    if (extends.empty()) {
      f_header_ << " private:" << endl;
      // 1: one parameter
      f_header_ << indent() << "void setProtocol" << short_suffix << "(" << prot_ptr << " prot) {"
                << endl;
      f_header_ << indent() << "setProtocol" << short_suffix << "(prot,prot);" << endl;
      f_header_ << indent() << "}" << endl;
      // 2: two parameter
      f_header_ << indent() << "void setProtocol" << short_suffix << "(" << prot_ptr << " iprot, "
                << prot_ptr << " oprot) {" << endl;

      f_header_ << indent() << "  piprot_=iprot;" << endl << indent() << "  poprot_=oprot;" << endl
                << indent() << "  iprot_ = iprot.get();" << endl << indent()
                << "  oprot_ = oprot.get();" << endl;

      f_header_ << indent() << "}" << endl;
      f_header_ << " public:" << endl;
    }

    // Generate getters for the protocols.
    // Note that these are not currently templated for simplicity.
    // TODO(simpkins): should they be templated?
    f_header_ << indent()
              << "boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {"
              << endl << indent() << "  return " << _this << "piprot_;" << endl << indent() << "}"
              << endl;

    f_header_ << indent()
              << "boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {"
              << endl << indent() << "  return " << _this << "poprot_;" << endl << indent() << "}"
              << endl;

  } else /* if (style == "Cob") */ {
    f_header_ << indent() << service_name_ << style << "Client" << short_suffix << "("
              << "boost::shared_ptr< ::apache::thrift::async::TAsyncChannel> channel, "
              << "::apache::thrift::protocol::TProtocolFactory* protocolFactory) :" << endl;
    if (extends.empty()) {
      f_header_ << indent() << "  channel_(channel)," << endl << indent()
                << "  itrans_(new ::apache::thrift::transport::TMemoryBuffer())," << endl
                << indent() << "  otrans_(new ::apache::thrift::transport::TMemoryBuffer()),"
                << endl;
      if (gen_templates_) {
        // TProtocolFactory classes return generic TProtocol pointers.
        // We have to dynamic cast to the Protocol_ type we are expecting.
        f_header_ << indent() << "  piprot_(boost::dynamic_pointer_cast<Protocol_>("
                  << "protocolFactory->getProtocol(itrans_)))," << endl << indent()
                  << "  poprot_(boost::dynamic_pointer_cast<Protocol_>("
                  << "protocolFactory->getProtocol(otrans_))) {" << endl;
        // Throw a TException if either dynamic cast failed.
        f_header_ << indent() << "  if (!piprot_ || !poprot_) {" << endl << indent()
                  << "    throw ::apache::thrift::TException(\""
                  << "TProtocolFactory returned unexpected protocol type in " << service_name_
                  << style << "Client" << short_suffix << " constructor\");" << endl << indent()
                  << "  }" << endl;
      } else {
        f_header_ << indent() << "  piprot_(protocolFactory->getProtocol(itrans_))," << endl
                  << indent() << "  poprot_(protocolFactory->getProtocol(otrans_)) {" << endl;
      }
      f_header_ << indent() << "  iprot_ = piprot_.get();" << endl << indent()
                << "  oprot_ = poprot_.get();" << endl << indent() << "}" << endl;
    } else {
      f_header_ << indent() << "  " << extends << style << client_suffix
                << "(channel, protocolFactory) {}" << endl;
    }
  }

  if (style == "Cob") {
    f_header_ << indent()
              << "boost::shared_ptr< ::apache::thrift::async::TAsyncChannel> getChannel() {" << endl
              << indent() << "  return " << _this << "channel_;" << endl << indent() << "}" << endl;
    if (!gen_no_client_completion_) {
      f_header_ << indent() << "virtual void completed__(bool /* success */) {}" << endl;
    }
  }

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    indent(f_header_) << function_signature(*f_iter, ifstyle) << ";" << endl;
    // TODO(dreiss): Use private inheritance to avoid generating thise in cob-style.
    t_function send_function(g_type_void,
                             string("send_") + (*f_iter)->get_name(),
                             (*f_iter)->get_arglist());
    indent(f_header_) << function_signature(&send_function, "") << ";" << endl;
    if (!(*f_iter)->is_oneway()) {
      t_struct noargs(program_);
      t_function recv_function((*f_iter)->get_returntype(),
                               string("recv_") + (*f_iter)->get_name(),
                               &noargs);
      indent(f_header_) << function_signature(&recv_function, "") << ";" << endl;
    }
  }
  indent_down();

  if (extends.empty()) {
    f_header_ << " protected:" << endl;
    indent_up();

    if (style == "Cob") {
      f_header_ << indent()
                << "boost::shared_ptr< ::apache::thrift::async::TAsyncChannel> channel_;" << endl
                << indent()
                << "boost::shared_ptr< ::apache::thrift::transport::TMemoryBuffer> itrans_;" << endl
                << indent()
                << "boost::shared_ptr< ::apache::thrift::transport::TMemoryBuffer> otrans_;"
                << endl;
    }
    f_header_ << indent() << prot_ptr << " piprot_;" << endl << indent() << prot_ptr << " poprot_;"
              << endl << indent() << protocol_type << "* iprot_;" << endl << indent()
              << protocol_type << "* oprot_;" << endl;

    indent_down();
  }

  f_header_ << "};" << endl << endl;

  if (gen_templates_) {
    // Output a backwards compatibility typedef using
    // TProtocol as the template parameter.
    f_header_ << "typedef " << service_name_ << style
              << "ClientT< ::apache::thrift::protocol::TProtocol> " << service_name_ << style
              << "Client;" << endl << endl;
  }

  string scope = service_name_ + style + client_suffix + "::";

  // Generate client method implementations
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    string funname = (*f_iter)->get_name();

    // Open function
    if (gen_templates_) {
      indent(out) << template_header;
    }
    indent(out) << function_signature(*f_iter, ifstyle, scope) << endl;
    scope_up(out);
    indent(out) << "send_" << funname << "(";

    // Get the struct of function call params
    t_struct* arg_struct = (*f_iter)->get_arglist();

    // Declare the function arguments
    const vector<t_field*>& fields = arg_struct->get_members();
    vector<t_field*>::const_iterator fld_iter;
    bool first = true;
    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      if (first) {
        first = false;
      } else {
        out << ", ";
      }
      out << (*fld_iter)->get_name();
    }
    out << ");" << endl;

    if (style != "Cob") {
      if (!(*f_iter)->is_oneway()) {
        out << indent();
        if (!(*f_iter)->get_returntype()->is_void()) {
          if (is_complex_type((*f_iter)->get_returntype())) {
            out << "recv_" << funname << "(_return);" << endl;
          } else {
            out << "return recv_" << funname << "();" << endl;
          }
        } else {
          out << "recv_" << funname << "();" << endl;
        }
      }
    } else {
      if (!(*f_iter)->is_oneway()) {
        out << indent() << _this << "channel_->sendAndRecvMessage("
            << "tcxx::bind(cob, this), " << _this << "otrans_.get(), " << _this << "itrans_.get());"
            << endl;
      } else {
        out << indent() << _this << "channel_->sendMessage("
            << "tcxx::bind(cob, this), " << _this << "otrans_.get());" << endl;
      }
    }
    scope_down(out);
    out << endl;

    // if (style != "Cob") // TODO(dreiss): Libify the client and don't generate this for cob-style
    if (true) {
      // Function for sending
      t_function send_function(g_type_void,
                               string("send_") + (*f_iter)->get_name(),
                               (*f_iter)->get_arglist());

      // Open the send function
      if (gen_templates_) {
        indent(out) << template_header;
      }
      indent(out) << function_signature(&send_function, "", scope) << endl;
      scope_up(out);

      // Function arguments and results
      string argsname = tservice->get_name() + "_" + (*f_iter)->get_name() + "_pargs";
      string resultname = tservice->get_name() + "_" + (*f_iter)->get_name() + "_presult";

      // Serialize the request
      out << indent() << "int32_t cseqid = 0;" << endl << indent() << _this
          << "oprot_->writeMessageBegin(\"" << (*f_iter)->get_name()
          << "\", ::apache::thrift::protocol::" << ((*f_iter)->is_oneway() ? "T_ONEWAY" : "T_CALL")
          << ", cseqid);" << endl << endl << indent() << argsname << " args;" << endl;

      for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
        out << indent() << "args." << (*fld_iter)->get_name() << " = &" << (*fld_iter)->get_name()
            << ";" << endl;
      }

      out << indent() << "args.write(" << _this << "oprot_);" << endl << endl << indent() << _this
          << "oprot_->writeMessageEnd();" << endl << indent() << _this
          << "oprot_->getTransport()->writeEnd();" << endl << indent() << _this
          << "oprot_->getTransport()->flush();" << endl;

      scope_down(out);
      out << endl;

      // Generate recv function only if not an oneway function
      if (!(*f_iter)->is_oneway()) {
        t_struct noargs(program_);
        t_function recv_function((*f_iter)->get_returntype(),
                                 string("recv_") + (*f_iter)->get_name(),
                                 &noargs);
        // Open the recv function
        if (gen_templates_) {
          indent(out) << template_header;
        }
        indent(out) << function_signature(&recv_function, "", scope) << endl;
        scope_up(out);

        out << endl << indent() << "int32_t rseqid = 0;" << endl << indent() << "std::string fname;"
            << endl << indent() << "::apache::thrift::protocol::TMessageType mtype;" << endl;
        if (style == "Cob" && !gen_no_client_completion_) {
          out << indent() << "bool completed = false;" << endl << endl << indent() << "try {";
          indent_up();
        }
        out << endl << indent() << _this << "iprot_->readMessageBegin(fname, mtype, rseqid);"
            << endl << indent() << "if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {" << endl
            << indent() << "  ::apache::thrift::TApplicationException x;" << endl << indent()
            << "  x.read(" << _this << "iprot_);" << endl << indent() << "  " << _this
            << "iprot_->readMessageEnd();" << endl << indent() << "  " << _this
            << "iprot_->getTransport()->readEnd();" << endl;
        if (style == "Cob" && !gen_no_client_completion_) {
          out << indent() << "  completed = true;" << endl << indent() << "  completed__(true);"
              << endl;
        }
        out << indent() << "  throw x;" << endl << indent() << "}" << endl << indent()
            << "if (mtype != ::apache::thrift::protocol::T_REPLY) {" << endl << indent() << "  "
            << _this << "iprot_->skip("
            << "::apache::thrift::protocol::T_STRUCT);" << endl << indent() << "  " << _this
            << "iprot_->readMessageEnd();" << endl << indent() << "  " << _this
            << "iprot_->getTransport()->readEnd();" << endl;
        if (style == "Cob" && !gen_no_client_completion_) {
          out << indent() << "  completed = true;" << endl << indent() << "  completed__(false);"
              << endl;
        }
        out << indent() << "}" << endl << indent() << "if (fname.compare(\""
            << (*f_iter)->get_name() << "\") != 0) {" << endl << indent() << "  " << _this
            << "iprot_->skip("
            << "::apache::thrift::protocol::T_STRUCT);" << endl << indent() << "  " << _this
            << "iprot_->readMessageEnd();" << endl << indent() << "  " << _this
            << "iprot_->getTransport()->readEnd();" << endl;
        if (style == "Cob" && !gen_no_client_completion_) {
          out << indent() << "  completed = true;" << endl << indent() << "  completed__(false);"
              << endl;
        }
        out << indent() << "}" << endl;

        if (!(*f_iter)->get_returntype()->is_void()
            && !is_complex_type((*f_iter)->get_returntype())) {
          t_field returnfield((*f_iter)->get_returntype(), "_return");
          out << indent() << declare_field(&returnfield) << endl;
        }

        out << indent() << resultname << " result;" << endl;

        if (!(*f_iter)->get_returntype()->is_void()) {
          out << indent() << "result.success = &_return;" << endl;
        }

        out << indent() << "result.read(" << _this << "iprot_);" << endl << indent() << _this
            << "iprot_->readMessageEnd();" << endl << indent() << _this
            << "iprot_->getTransport()->readEnd();" << endl << endl;

        // Careful, only look for _result if not a void function
        if (!(*f_iter)->get_returntype()->is_void()) {
          if (is_complex_type((*f_iter)->get_returntype())) {
            out << indent() << "if (result.__isset.success) {" << endl << indent()
                << "  // _return pointer has now been filled" << endl;
            if (style == "Cob" && !gen_no_client_completion_) {
              out << indent() << "  completed = true;" << endl << indent() << "  completed__(true);"
                  << endl;
            }
            out << indent() << "  return;" << endl << indent() << "}" << endl;
          } else {
            out << indent() << "if (result.__isset.success) {" << endl;
            if (style == "Cob" && !gen_no_client_completion_) {
              out << indent() << "  completed = true;" << endl << indent() << "  completed__(true);"
                  << endl;
            }
            out << indent() << "  return _return;" << endl << indent() << "}" << endl;
          }
        }

        t_struct* xs = (*f_iter)->get_xceptions();
        const std::vector<t_field*>& xceptions = xs->get_members();
        vector<t_field*>::const_iterator x_iter;
        for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
          out << indent() << "if (result.__isset." << (*x_iter)->get_name() << ") {" << endl;
          if (style == "Cob" && !gen_no_client_completion_) {
            out << indent() << "  completed = true;" << endl << indent() << "  completed__(true);"
                << endl;
          }
          out << indent() << "  throw result." << (*x_iter)->get_name() << ";" << endl << indent()
              << "}" << endl;
        }

        // We only get here if we are a void function
        if ((*f_iter)->get_returntype()->is_void()) {
          if (style == "Cob" && !gen_no_client_completion_) {
            out << indent() << "completed = true;" << endl << indent() << "completed__(true);"
                << endl;
          }
          indent(out) << "return;" << endl;
        } else {
          if (style == "Cob" && !gen_no_client_completion_) {
            out << indent() << "completed = true;" << endl << indent() << "completed__(true);"
                << endl;
          }
          out << indent() << "throw "
                             "::apache::thrift::TApplicationException(::apache::thrift::"
                             "TApplicationException::MISSING_RESULT, \"" << (*f_iter)->get_name()
              << " failed: unknown result\");" << endl;
        }
        if (style == "Cob" && !gen_no_client_completion_) {
          indent_down();
          out << indent() << "} catch (...) {" << endl << indent() << "  if (!completed) {" << endl
              << indent() << "    completed__(false);" << endl << indent() << "  }" << endl
              << indent() << "  throw;" << endl << indent() << "}" << endl;
        }
        // Close function
        scope_down(out);
        out << endl;
      }
    }
  }
}