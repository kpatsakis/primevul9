quantify_node(Node **np, int lower, int upper)
{
  Node* tmp = node_new_quantifier(lower, upper, 0);
  if (IS_NULL(tmp)) return ONIGERR_MEMORY;
  NQTFR(tmp)->target = *np;
  *np = tmp;
  return 0;
}