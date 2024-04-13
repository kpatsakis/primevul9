tune_next(Node* node, Node* next_node, regex_t* reg)
{
  int called;
  NodeType type;

  called = FALSE;

 retry:
  type = NODE_TYPE(node);
  if (type == NODE_QUANT) {
    QuantNode* qn = QUANT_(node);
    if (qn->greedy && IS_INFINITE_REPEAT(qn->upper)) {
#ifdef USE_QUANT_PEEK_NEXT
      if (called == FALSE) {
        Node* n = get_tree_head_literal(next_node, 1, reg);
        /* '\0': for UTF-16BE etc... */
        if (IS_NOT_NULL(n) && STR_(n)->s[0] != '\0') {
          qn->next_head_exact = n;
        }
      }
#endif
      /* automatic posseivation a*b ==> (?>a*)b */
      if (qn->lower <= 1) {
        if (is_strict_real_node(NODE_BODY(node))) {
          Node *x, *y;
          x = get_tree_head_literal(NODE_BODY(node), 0, reg);
          if (IS_NOT_NULL(x)) {
            y = get_tree_head_literal(next_node,  0, reg);
            if (IS_NOT_NULL(y) && is_exclusive(x, y, reg)) {
              Node* en = onig_node_new_bag(BAG_STOP_BACKTRACK);
              CHECK_NULL_RETURN_MEMERR(en);
              NODE_STATUS_ADD(en, STRICT_REAL_REPEAT);
              node_swap(node, en);
              NODE_BODY(node) = en;
            }
          }
        }
      }
    }
  }
  else if (type == NODE_BAG) {
    BagNode* en = BAG_(node);
    if (en->type == BAG_MEMORY) {
      if (NODE_IS_CALLED(node))
        called = TRUE;
      node = NODE_BODY(node);
      goto retry;
    }
  }
  return 0;
}