onig_node_new_anchor(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_ANCHOR);
  ANCHOR_(node)->type     = type;
  ANCHOR_(node)->char_len = -1;
  return node;
}