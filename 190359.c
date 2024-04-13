void t_cpp_generator::generate_move_assignment_operator(ofstream& out, t_struct* tstruct) {
  generate_assignment_helper(out, tstruct, /*is_move=*/true);
}