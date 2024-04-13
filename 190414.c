void ProcessorGenerator::generate_factory() {
  string if_factory_name = if_name_ + "Factory";

  // Generate the factory class definition
  f_header_ << template_header_ << "class " << factory_class_name_ << " : public ::apache::thrift::"
            << (style_ == "Cob" ? "async::TAsyncProcessorFactory" : "TProcessorFactory") << " {"
            << endl << " public:" << endl;
  indent_up();

  f_header_ << indent() << factory_class_name_ << "(const ::boost::shared_ptr< " << if_factory_name
            << " >& handlerFactory) :" << endl << indent()
            << "    handlerFactory_(handlerFactory) {}" << endl << endl << indent()
            << "::boost::shared_ptr< ::apache::thrift::"
            << (style_ == "Cob" ? "async::TAsyncProcessor" : "TProcessor") << " > "
            << "getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);" << endl;

  f_header_ << endl << " protected:" << endl << indent() << "::boost::shared_ptr< "
            << if_factory_name << " > handlerFactory_;" << endl;

  indent_down();
  f_header_ << "};" << endl << endl;

  // If we are generating templates, output a typedef for the plain
  // factory name.
  if (generator_->gen_templates_) {
    f_header_ << "typedef " << factory_class_name_
              << "< ::apache::thrift::protocol::TDummyProtocol > " << service_name_ << pstyle_
              << "ProcessorFactory;" << endl << endl;
  }

  // Generate the getProcessor() method
  f_out_ << template_header_ << indent() << "::boost::shared_ptr< ::apache::thrift::"
         << (style_ == "Cob" ? "async::TAsyncProcessor" : "TProcessor") << " > "
         << factory_class_name_ << template_suffix_ << "::getProcessor("
         << "const ::apache::thrift::TConnectionInfo& connInfo) {" << endl;
  indent_up();

  f_out_ << indent() << "::apache::thrift::ReleaseHandler< " << if_factory_name
         << " > cleanup(handlerFactory_);" << endl << indent() << "::boost::shared_ptr< "
         << if_name_ << " > handler("
         << "handlerFactory_->getHandler(connInfo), cleanup);" << endl << indent()
         << "::boost::shared_ptr< ::apache::thrift::"
         << (style_ == "Cob" ? "async::TAsyncProcessor" : "TProcessor") << " > "
         << "processor(new " << class_name_ << template_suffix_ << "(handler));" << endl << indent()
         << "return processor;" << endl;

  indent_down();
  f_out_ << indent() << "}" << endl;
}