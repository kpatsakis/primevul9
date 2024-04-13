compile_length_option_node(BagNode* node, regex_t* reg)
{
  int tlen;

  tlen = compile_length_tree(NODE_BAG_BODY(node), reg);

  return tlen;
}