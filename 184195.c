node_reduce_in_look_behind(Node* node)
{
  NodeType type;
  Node* body;

  if (NODE_TYPE(node) != NODE_QUANT) return 0;

  body = NODE_BODY(node);
  type = NODE_TYPE(body);
  if (type == NODE_STRING || type == NODE_CTYPE ||
      type == NODE_CCLASS || type == NODE_BACKREF) {
    QuantNode* qn = QUANT_(node);
    qn->upper = qn->lower;
    if (qn->upper == 0)
      return 1; /* removed */
  }

  return 0;
}