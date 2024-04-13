compile_option_node(BagNode* node, regex_t* reg, ScanEnv* env)
{
  int r;

  r = compile_tree(NODE_BAG_BODY(node), reg, env);

  return r;
}