node_list_len(Node* list)
{
  int len;

  len = 1;
  while (IS_NOT_NULL(NODE_CDR(list))) {
    list = NODE_CDR(list);
    len++;
  }

  return len;
}