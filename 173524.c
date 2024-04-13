node_new_ctype(int type, int not)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_CTYPE);
  CTYPE_(node)->ctype = type;
  CTYPE_(node)->not   = not;
  return node;
}