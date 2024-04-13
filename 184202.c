node_char_len(Node* node, regex_t* reg, MinMaxCharLen* ci, ScanEnv* env)
{
  return node_char_len1(node, reg, ci, env, 0);
}