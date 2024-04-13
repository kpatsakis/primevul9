node_new_option(OnigOptionType option)
{
  Node* node = node_new_enclosure(ENCLOSURE_OPTION);
  CHECK_NULL_RETURN(node);
  ENCLOSURE_(node)->option = option;
  return node;
}