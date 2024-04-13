void t_cpp_generator::generate_service(t_service* tservice) {
  string svcname = tservice->get_name();

  // Make output files
  string f_header_name = get_out_dir() + svcname + ".h";
  f_header_.open(f_header_name.c_str());

  // Print header file includes
  f_header_ << autogen_comment();
  f_header_ << "#ifndef " << svcname << "_H" << endl << "#define " << svcname << "_H" << endl
            << endl;
  if (gen_cob_style_) {
    f_header_ << "#include <thrift/transport/TBufferTransports.h>" << endl << // TMemoryBuffer
        "#include <thrift/cxxfunctional.h>" << endl
              << "namespace apache { namespace thrift { namespace async {" << endl
              << "class TAsyncChannel;" << endl << "}}}" << endl;
  }
  f_header_ << "#include <thrift/TDispatchProcessor.h>" << endl;
  if (gen_cob_style_) {
    f_header_ << "#include <thrift/async/TAsyncDispatchProcessor.h>" << endl;
  }
  f_header_ << "#include \"" << get_include_prefix(*get_program()) << program_name_ << "_types.h\""
            << endl;

  t_service* extends_service = tservice->get_extends();
  if (extends_service != NULL) {
    f_header_ << "#include \"" << get_include_prefix(*(extends_service->get_program()))
              << extends_service->get_name() << ".h\"" << endl;
  }

  f_header_ << endl << ns_open_ << endl << endl;

  // Service implementation file includes
  string f_service_name = get_out_dir() + svcname + ".cpp";
  f_service_.open(f_service_name.c_str());
  f_service_ << autogen_comment();
  f_service_ << "#include \"" << get_include_prefix(*get_program()) << svcname << ".h\"" << endl;
  if (gen_cob_style_) {
    f_service_ << "#include \"thrift/async/TAsyncChannel.h\"" << endl;
  }
  if (gen_templates_) {
    f_service_ << "#include \"" << get_include_prefix(*get_program()) << svcname << ".tcc\""
               << endl;

    string f_service_tcc_name = get_out_dir() + svcname + ".tcc";
    f_service_tcc_.open(f_service_tcc_name.c_str());
    f_service_tcc_ << autogen_comment();
    f_service_tcc_ << "#include \"" << get_include_prefix(*get_program()) << svcname << ".h\""
                   << endl;

    f_service_tcc_ << "#ifndef " << svcname << "_TCC" << endl << "#define " << svcname << "_TCC"
                   << endl << endl;

    if (gen_cob_style_) {
      f_service_tcc_ << "#include \"thrift/async/TAsyncChannel.h\"" << endl;
    }
  }

  f_service_ << endl << ns_open_ << endl << endl;
  f_service_tcc_ << endl << ns_open_ << endl << endl;

  // Generate all the components
  generate_service_interface(tservice, "");
  generate_service_interface_factory(tservice, "");
  generate_service_null(tservice, "");
  generate_service_helpers(tservice);
  generate_service_client(tservice, "");
  generate_service_processor(tservice, "");
  generate_service_multiface(tservice);
  generate_service_skeleton(tservice);

  // Generate all the cob components
  if (gen_cob_style_) {
    generate_service_interface(tservice, "CobCl");
    generate_service_interface(tservice, "CobSv");
    generate_service_interface_factory(tservice, "CobSv");
    generate_service_null(tservice, "CobSv");
    generate_service_client(tservice, "Cob");
    generate_service_processor(tservice, "Cob");
    generate_service_async_skeleton(tservice);
  }

  // Close the namespace
  f_service_ << ns_close_ << endl << endl;
  f_service_tcc_ << ns_close_ << endl << endl;
  f_header_ << ns_close_ << endl << endl;

  // TODO(simpkins): Make this a separate option
  if (gen_templates_) {
    f_header_ << "#include \"" << get_include_prefix(*get_program()) << svcname << ".tcc\"" << endl
              << "#include \"" << get_include_prefix(*get_program()) << program_name_
              << "_types.tcc\"" << endl << endl;
  }

  f_header_ << "#endif" << endl;
  f_service_tcc_ << "#endif" << endl;

  // Close the files
  f_service_tcc_.close();
  f_service_.close();
  f_header_.close();
}