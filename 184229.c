compile_tree_n_times(Node* node, int n, regex_t* reg, ScanEnv* env)
{
  int i, r;

  for (i = 0; i < n; i++) {
    r = compile_tree(node, reg, env);
    if (r != 0) return r;
  }
  return 0;
}