node_new_quantifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_QUANT);
  QUANT_(node)->lower  = lower;
  QUANT_(node)->upper  = upper;
  QUANT_(node)->greedy = 1;
  QUANT_(node)->body_empty_info = QUANT_BODY_IS_NOT_EMPTY;
  QUANT_(node)->head_exact      = NULL_NODE;
  QUANT_(node)->next_head_exact = NULL_NODE;
  QUANT_(node)->is_refered      = 0;
  if (by_number != 0)
    NODE_STATUS_ADD(node, NST_BY_NUMBER);

  return node;
}