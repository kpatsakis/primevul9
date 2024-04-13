onig_node_free(Node* node)
{
 start:
  if (IS_NULL(node)) return ;

#ifdef DEBUG_NODE_FREE
  fprintf(stderr, "onig_node_free: %p\n", node);
#endif

  switch (NODE_TYPE(node)) {
  case NODE_STR:
    if (STR_(node)->capa != 0 &&
        IS_NOT_NULL(STR_(node)->s) && STR_(node)->s != STR_(node)->buf) {
      xfree(STR_(node)->s);
    }
    break;

  case NODE_LIST:
  case NODE_ALT:
    onig_node_free(NODE_CAR(node));
    {
      Node* next_node = NODE_CDR(node);

      xfree(node);
      node = next_node;
      goto start;
    }
    break;

  case NODE_CCLASS:
    {
      CClassNode* cc = CCLASS_(node);

      if (IS_NCCLASS_SHARE(cc)) return ;
      if (cc->mbuf)
        bbuf_free(cc->mbuf);
    }
    break;

  case NODE_BREF:
    if (IS_NOT_NULL(BREF_(node)->back_dynamic))
      xfree(BREF_(node)->back_dynamic);
    break;

  case NODE_QTFR:
  case NODE_ENCLOSURE:
  case NODE_ANCHOR:
    if (NODE_BODY(node))
      onig_node_free(NODE_BODY(node));
    break;

  case NODE_CTYPE:
  case NODE_CALL:
    break;
  }

  xfree(node);
}