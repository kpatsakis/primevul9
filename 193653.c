compile_length_option_node(EnclosureNode* node, regex_t* reg)
{
  int tlen;
  OnigOptionType prev = reg->options;

  reg->options = node->o.options;
  tlen = compile_length_tree(NODE_ENCLOSURE_BODY(node), reg);
  reg->options = prev;

  if (tlen < 0) return tlen;

  if (IS_DYNAMIC_OPTION(prev ^ node->option)) {
    return SIZE_OP_SET_OPTION_PUSH + SIZE_OP_SET_OPTION + SIZE_OP_FAIL
           + tlen + SIZE_OP_SET_OPTION;
  }
  else
    return tlen;
}