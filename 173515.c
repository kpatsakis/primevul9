parse_branch(Node** top, OnigToken* tok, int term,
	     UChar** src, UChar* end, ScanEnv* env)
{
  int r;
  Node *node, **headp;

  *top = NULL;
  r = parse_exp(&node, tok, term, src, end, env);
  if (r < 0) {
    onig_node_free(node);
    return r;
  }

  if (r == TK_EOT || r == term || r == TK_ALT) {
    *top = node;
  }
  else {
    *top  = node_new_list(node, NULL);
    headp = &(NODE_CDR(*top));
    while (r != TK_EOT && r != term && r != TK_ALT) {
      r = parse_exp(&node, tok, term, src, end, env);
      if (r < 0) {
        onig_node_free(node);
        return r;
      }

      if (NODE_TYPE(node) == NODE_LIST) {
        *headp = node;
        while (IS_NOT_NULL(NODE_CDR(node))) node = NODE_CDR(node);
        headp = &(NODE_CDR(node));
      }
      else {
        *headp = node_new_list(node, NULL);
        headp = &(NODE_CDR(*headp));
      }
    }
  }

  return r;
}