list_reduce_in_look_behind(Node* node)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_QUANT:
    r = node_reduce_in_look_behind(node);
    if (r > 0) r = 0;
    break;

  case NODE_LIST:
    do {
      r = node_reduce_in_look_behind(NODE_CAR(node));
      if (r <= 0) break;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  default:
    r = 0;
    break;
  }

  return r;
}