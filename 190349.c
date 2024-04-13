void t_cpp_generator::generate_copy_constructor(ofstream& out,
                                                t_struct* tstruct,
                                                bool is_exception) {
  generate_constructor_helper(out, tstruct, is_exception, /*is_move=*/false);
}