remove_from_list(Node* prev, Node* a)
{
  if (NODE_CDR(prev) != a) return ;

  NODE_CDR(prev) = NODE_CDR(a);
  NODE_CDR(a) = NULL_NODE;
}