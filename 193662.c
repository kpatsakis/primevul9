compile_option_node(EnclosureNode* node, regex_t* reg, ScanEnv* env)
{
  int r;
  OnigOptionType prev = reg->options;

  if (IS_DYNAMIC_OPTION(prev ^ node->o.options)) {
    r = add_opcode_option(reg, OP_SET_OPTION_PUSH, node->o.options);
    if (r != 0) return r;
    r = add_opcode_option(reg, OP_SET_OPTION, prev);
    if (r != 0) return r;
    r = add_opcode(reg, OP_FAIL);
    if (r != 0) return r;
  }

  reg->options = node->o.options;
  r = compile_tree(NODE_ENCLOSURE_BODY(node), reg, env);
  reg->options = prev;

  if (IS_DYNAMIC_OPTION(prev ^ node->o.options)) {
    if (r != 0) return r;
    r = add_opcode_option(reg, OP_SET_OPTION, prev);
  }
  return r;
}