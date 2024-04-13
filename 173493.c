node_new_quantifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NODE_TYPE(node, NODE_QTFR);
  QTFR_(node)->lower  = lower;
  QTFR_(node)->upper  = upper;
  QTFR_(node)->greedy = 1;
  QTFR_(node)->body_empty_info = NQ_BODY_ISNOT_EMPTY;
  QTFR_(node)->head_exact      = NULL_NODE;
  QTFR_(node)->next_head_exact = NULL_NODE;
  QTFR_(node)->is_refered      = 0;
  if (by_number != 0)
    NODE_STATUS_ADD(node, NST_BY_NUMBER);

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  QTFR_(node)->comb_exp_check_num = 0;
#endif

  return node;
}