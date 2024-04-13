void t_cpp_generator::generate_typedef(t_typedef* ttypedef) {
  f_types_ << indent() << "typedef " << type_name(ttypedef->get_type(), true) << " "
           << ttypedef->get_symbolic() << ";" << endl << endl;
}