create_node_from_array(int kind, Node **np, Node **node_array)
{
  Node* tmp = NULL_NODE;
  int i = 0;

  while (node_array[i] != NULL_NODE)  i++;
  while (--i >= 0) {
    *np = kind==LIST ? node_new_list(node_array[i], tmp)
                     : onig_node_new_alt(node_array[i], tmp);
    if (IS_NULL(*np)) {
      while (i >= 0) {
        onig_node_free(node_array[i]);
        node_array[i--] = NULL_NODE;
      }
      onig_node_free(tmp);
      return ONIGERR_MEMORY;
    }
    else
      node_array[i] = NULL_NODE;
    tmp = *np;
  }
  return 0;
}