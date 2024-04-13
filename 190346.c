void t_cpp_generator::init_generator() {
  // Make output directory
  MKDIR(get_out_dir().c_str());

  // Make output file
  string f_types_name = get_out_dir() + program_name_ + "_types.h";
  f_types_.open(f_types_name.c_str());

  string f_types_impl_name = get_out_dir() + program_name_ + "_types.cpp";
  f_types_impl_.open(f_types_impl_name.c_str());

  if (gen_templates_) {
    // If we don't open the stream, it appears to just discard data,
    // which is fine.
    string f_types_tcc_name = get_out_dir() + program_name_ + "_types.tcc";
    f_types_tcc_.open(f_types_tcc_name.c_str());
  }

  // Print header
  f_types_ << autogen_comment();
  f_types_impl_ << autogen_comment();
  f_types_tcc_ << autogen_comment();

  // Start ifndef
  f_types_ << "#ifndef " << program_name_ << "_TYPES_H" << endl << "#define " << program_name_
           << "_TYPES_H" << endl << endl;
  f_types_tcc_ << "#ifndef " << program_name_ << "_TYPES_TCC" << endl << "#define " << program_name_
               << "_TYPES_TCC" << endl << endl;

  // Include base types
  f_types_ << "#include <iosfwd>" << endl << endl << "#include <thrift/Thrift.h>" << endl
           << "#include <thrift/TApplicationException.h>" << endl
           << "#include <thrift/protocol/TProtocol.h>" << endl
           << "#include <thrift/transport/TTransport.h>" << endl << endl;
  // Include C++xx compatibility header
  f_types_ << "#include <thrift/cxxfunctional.h>" << endl;

  // Include other Thrift includes
  const vector<t_program*>& includes = program_->get_includes();
  for (size_t i = 0; i < includes.size(); ++i) {
    f_types_ << "#include \"" << get_include_prefix(*(includes[i])) << includes[i]->get_name()
             << "_types.h\"" << endl;

    // XXX(simpkins): If gen_templates_ is enabled, we currently assume all
    // included files were also generated with templates enabled.
    f_types_tcc_ << "#include \"" << get_include_prefix(*(includes[i])) << includes[i]->get_name()
                 << "_types.tcc\"" << endl;
  }
  f_types_ << endl;

  // Include custom headers
  const vector<string>& cpp_includes = program_->get_cpp_includes();
  for (size_t i = 0; i < cpp_includes.size(); ++i) {
    if (cpp_includes[i][0] == '<') {
      f_types_ << "#include " << cpp_includes[i] << endl;
    } else {
      f_types_ << "#include \"" << cpp_includes[i] << "\"" << endl;
    }
  }
  f_types_ << endl;

  // Include the types file
  f_types_impl_ << "#include \"" << get_include_prefix(*get_program()) << program_name_
                << "_types.h\"" << endl << endl;
  f_types_tcc_ << "#include \"" << get_include_prefix(*get_program()) << program_name_
               << "_types.h\"" << endl << endl;

  // If we are generating local reflection metadata, we need to include
  // the definition of TypeSpec.
  if (gen_dense_) {
    f_types_impl_ << "#include <thrift/TReflectionLocal.h>" << endl << endl;
  }

  // The swap() code needs <algorithm> for std::swap()
  f_types_impl_ << "#include <algorithm>" << endl;
  // for operator<<
  f_types_impl_ << "#include <ostream>" << endl << endl;
  f_types_impl_ << "#include <thrift/TToString.h>" << endl << endl;

  // Open namespace
  ns_open_ = namespace_open(program_->get_namespace("cpp"));
  ns_close_ = namespace_close(program_->get_namespace("cpp"));

  f_types_ << ns_open_ << endl << endl;

  f_types_impl_ << ns_open_ << endl << endl;

  f_types_tcc_ << ns_open_ << endl << endl;
}