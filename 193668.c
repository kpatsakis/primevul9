next_setup(Node* node, Node* next_node, regex_t* reg)
{
  NodeType type;

 retry:
  type = NODE_TYPE(node);
  if (type == NODE_QUANT) {
    QuantNode* qn = QUANT_(node);
    if (qn->greedy && IS_REPEAT_INFINITE(qn->upper)) {
#ifdef USE_QUANT_PEEK_NEXT
      Node* n = get_head_value_node(next_node, 1, reg);
      /* '\0': for UTF-16BE etc... */
      if (IS_NOT_NULL(n) && STR_(n)->s[0] != '\0') {
        qn->next_head_exact = n;
      }
#endif
      /* automatic posseivation a*b ==> (?>a*)b */
      if (qn->lower <= 1) {
        if (NODE_IS_SIMPLE_TYPE(NODE_BODY(node))) {
          Node *x, *y;
          x = get_head_value_node(NODE_BODY(node), 0, reg);
          if (IS_NOT_NULL(x)) {
            y = get_head_value_node(next_node,  0, reg);
            if (IS_NOT_NULL(y) && is_exclusive(x, y, reg)) {
              Node* en = onig_node_new_enclosure(ENCLOSURE_STOP_BACKTRACK);
              CHECK_NULL_RETURN_MEMERR(en);
              NODE_STATUS_ADD(en, NST_STOP_BT_SIMPLE_REPEAT);
              swap_node(node, en);
              NODE_BODY(node) = en;
            }
          }
        }
      }
    }
  }
  else if (type == NODE_ENCLOSURE) {
    EnclosureNode* en = ENCLOSURE_(node);
    if (en->type == ENCLOSURE_MEMORY) {
      node = NODE_BODY(node);
      goto retry;
    }
  }
  return 0;
}