node_new_cclass(void)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_CCLASS);
  initialize_cclass(CCLASS_(node));
  return node;
}