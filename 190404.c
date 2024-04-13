void t_cpp_generator::generate_exception_what_method_decl(std::ofstream& out,
                                                          t_struct* tstruct,
                                                          bool external) {
  out << "const char* ";
  if (external) {
    out << tstruct->get_name() << "::";
  }
  out << "what() const throw()";
}