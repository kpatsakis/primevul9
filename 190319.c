void t_cpp_generator::generate_forward_declaration(t_struct* tstruct) {
  // Forward declare struct def
  f_types_ << indent() << "class " << tstruct->get_name() << ";" << endl << endl;
}