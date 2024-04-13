unravel_cf_node_add(Node** rlist, Node* add)
{
  Node *list;

  list = *rlist;
  if (IS_NULL(list)) {
    list = onig_node_new_list(add, NULL);
    CHECK_NULL_RETURN_MEMERR(list);
    *rlist = list;
  }
  else {
    Node* r = node_list_add(list, add);
    CHECK_NULL_RETURN_MEMERR(r);
  }

  return 0;
}