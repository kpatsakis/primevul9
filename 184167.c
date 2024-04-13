node_list_add(Node* list, Node* x)
{
  Node *n;

  n = onig_node_new_list(x, NULL);
  if (IS_NULL(n)) return NULL_NODE;

  if (IS_NOT_NULL(list)) {
    while (IS_NOT_NULL(NODE_CDR(list)))
      list = NODE_CDR(list);

    NODE_CDR(list) = n;
  }

  return n;
}