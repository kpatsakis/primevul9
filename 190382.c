void t_cpp_generator::close_generator() {
  // Close namespace
  f_types_ << ns_close_ << endl << endl;
  f_types_impl_ << ns_close_ << endl;
  f_types_tcc_ << ns_close_ << endl << endl;

  // Include the types.tcc file from the types header file,
  // so clients don't have to explicitly include the tcc file.
  // TODO(simpkins): Make this a separate option.
  if (gen_templates_) {
    f_types_ << "#include \"" << get_include_prefix(*get_program()) << program_name_
             << "_types.tcc\"" << endl << endl;
  }

  // Close ifndef
  f_types_ << "#endif" << endl;
  f_types_tcc_ << "#endif" << endl;

  // Close output file
  f_types_.close();
  f_types_impl_.close();
  f_types_tcc_.close();
}