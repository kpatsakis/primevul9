node_str_node_cat(Node* node, Node* add)
{
  int r;

  if (NODE_STATUS(node) != NODE_STATUS(add))
    return ONIGERR_TYPE_BUG;

  if (STR_(node)->flag != STR_(add)->flag)
    return ONIGERR_TYPE_BUG;

  r = onig_node_str_cat(node, STR_(add)->s, STR_(add)->end);
  if (r != 0) return r;

  return 0;
}