node_new_enclosure_memory(OnigOptionType option, int is_named)
{
  Node* node = node_new_enclosure(ENCLOSURE_MEMORY);
  CHECK_NULL_RETURN(node);
  if (is_named != 0)
    NODE_STATUS_ADD(node, NST_NAMED_GROUP);

#ifdef USE_SUBEXP_CALL
  ENCLOSURE_(node)->option = option;
#endif
  return node;
}