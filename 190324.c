void t_cpp_generator::generate_struct_ostream_operator_decl(std::ofstream& out, t_struct* tstruct) {
  out << "std::ostream& operator<<(std::ostream& out, const " << tstruct->get_name() << "& obj)";
}