node_new_quantifier(int lower, int upper, int by_number)
{
  Node* node = node_new();
  CHECK_NULL_RETURN(node);

  SET_NTYPE(node, NT_QTFR);
  NQTFR(node)->state  = 0;
  NQTFR(node)->target = NULL;
  NQTFR(node)->lower  = lower;
  NQTFR(node)->upper  = upper;
  NQTFR(node)->greedy = 1;
  NQTFR(node)->target_empty_info = NQ_TARGET_ISNOT_EMPTY;
  NQTFR(node)->head_exact        = NULL_NODE;
  NQTFR(node)->next_head_exact   = NULL_NODE;
  NQTFR(node)->is_referred       = 0;
  if (by_number != 0)
    NQTFR(node)->state |= NST_BY_NUMBER;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  NQTFR(node)->comb_exp_check_num = 0;
#endif

  return node;
}