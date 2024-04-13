node_new_enclosure(int type)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_ENCLOSURE);
  ENCLOSURE_(node)->type      = type;
  ENCLOSURE_(node)->regnum    =  0;
  ENCLOSURE_(node)->option    =  0;
  ENCLOSURE_(node)->call_addr = -1;
  ENCLOSURE_(node)->opt_count =  0;
  return node;
}