get_head_value_node(Node* node, int exact, regex_t* reg)
{
  Node* n = NULL_NODE;

  switch (NODE_TYPE(node)) {
  case NODE_BACKREF:
  case NODE_ALT:
#ifdef USE_CALL
  case NODE_CALL:
#endif
    break;

  case NODE_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR)
      break;
    /* fall */
  case NODE_CCLASS:
    if (exact == 0) {
      n = node;
    }
    break;

  case NODE_LIST:
    n = get_head_value_node(NODE_CAR(node), exact, reg);
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s)
        break;

      if (exact != 0 &&
          !NODE_STRING_IS_RAW(node) && IS_IGNORECASE(reg->options)) {
      }
      else {
        n = node;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower > 0) {
        if (IS_NOT_NULL(qn->head_exact))
          n = qn->head_exact;
        else
          n = get_head_value_node(NODE_BODY(node), exact, reg);
      }
    }
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);
      switch (en->type) {
      case ENCLOSURE_OPTION:
        {
          OnigOptionType options = reg->options;

          reg->options = ENCLOSURE_(node)->o.options;
          n = get_head_value_node(NODE_BODY(node), exact, reg);
          reg->options = options;
        }
        break;

      case ENCLOSURE_MEMORY:
      case ENCLOSURE_STOP_BACKTRACK:
      case ENCLOSURE_IF_ELSE:
        n = get_head_value_node(NODE_BODY(node), exact, reg);
        break;
      }
    }
    break;

  case NODE_ANCHOR:
    if (ANCHOR_(node)->type == ANCHOR_PREC_READ)
      n = get_head_value_node(NODE_BODY(node), exact, reg);
    break;

  case NODE_GIMMICK:
  default:
    break;
  }

  return n;
}