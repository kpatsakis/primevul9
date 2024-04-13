node_detect_can_be_slow(Node* node, SlowElementCount* ct)
{
  int r;

  r = 0;
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = node_detect_can_be_slow(NODE_CAR(node), ct);
      if (r != 0) return r;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = node_detect_can_be_slow(NODE_BODY(node), ct);
    break;

  case NODE_ANCHOR:
    switch (ANCHOR_(node)->type) {
    case ANCR_PREC_READ:
    case ANCR_PREC_READ_NOT:
      ct->prec_read++;
      break;
    case ANCR_LOOK_BEHIND:
    case ANCR_LOOK_BEHIND_NOT:
      ct->look_behind++;
      break;
    default:
      break;
    }

    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = node_detect_can_be_slow(NODE_BODY(node), ct);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = node_detect_can_be_slow(NODE_BODY(node), ct);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = node_detect_can_be_slow(en->te.Then, ct);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = node_detect_can_be_slow(en->te.Else, ct);
          if (r != 0) return r;
        }
      }
    }
    break;

#ifdef USE_BACKREF_WITH_LEVEL
  case NODE_BACKREF:
    if (NODE_IS_NEST_LEVEL(node))
      ct->backref_with_level++;
    break;
#endif

#ifdef USE_CALL
  case NODE_CALL:
    ct->call++;
    break;
#endif

  default:
    break;
  }

  return r;
}