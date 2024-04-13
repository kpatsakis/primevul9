void t_cpp_generator::generate_local_reflection_pointer(std::ofstream& out, t_type* ttype) {
  if (!gen_dense_) {
    return;
  }
  indent(out) << "::apache::thrift::reflection::local::TypeSpec* " << ttype->get_name()
              << "::local_reflection = " << endl << indent() << "  &"
              << local_reflection_name("typespec", ttype) << ";" << endl << endl;
}