divide_look_behind_alternatives(Node* node)
{
  int r;
  int anc_type;
  Node *head, *np, *insert_node;
  AnchorNode* an;

  an = ANCHOR_(node);
  anc_type = an->type;

  head = NODE_ANCHOR_BODY(an);
  np = NODE_CAR(head);
  node_swap(node, head);
  NODE_CAR(node) = head;
  NODE_BODY(head) = np;

  np = node;
  while (IS_NOT_NULL(np = NODE_CDR(np))) {
    r = onig_node_copy(&insert_node, head);
    if (r != 0) return r;
    CHECK_NULL_RETURN_MEMERR(insert_node);
    NODE_BODY(insert_node) = NODE_CAR(np);
    NODE_CAR(np) = insert_node;
  }

  if (anc_type == ANCR_LOOK_BEHIND_NOT) {
    np = node;
    do {
      NODE_SET_TYPE(np, NODE_LIST);  /* alt -> list */
    } while (IS_NOT_NULL(np = NODE_CDR(np)));
  }
  return 0;
}