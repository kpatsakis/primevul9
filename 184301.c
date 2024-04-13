node_conv_to_str_node(Node* node, Node* ref_node)
{
  xmemset(node, 0, sizeof(*node));
  NODE_SET_TYPE(node, NODE_STRING);
  NODE_STATUS(node) = NODE_STATUS(ref_node);

  STR_(node)->flag     = STR_(ref_node)->flag;
  STR_(node)->s        = STR_(node)->buf;
  STR_(node)->end      = STR_(node)->buf;
  STR_(node)->capacity = 0;
}