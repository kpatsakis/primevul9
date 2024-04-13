alt_reduce_in_look_behind(Node* node, regex_t* reg, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_ALT:
    do {
      r = list_reduce_in_look_behind(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  default:
    r = list_reduce_in_look_behind(node);
    break;
  }

  return r;
}