tune_called_state(Node* node, int state)
{
  switch (NODE_TYPE(node)) {
  case NODE_ALT:
    state |= IN_ALT;
    /* fall */
  case NODE_LIST:
    do {
      tune_called_state(NODE_CAR(node), state);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

#ifdef USE_CALL
  case NODE_CALL:
    tune_called_state_call(node, state);
    break;
#endif

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_MEMORY:
        if (en->m.entry_count > 1)
          state |= IN_MULTI_ENTRY;

        en->m.called_state |= state;
        /* fall */
      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        tune_called_state(NODE_BODY(node), state);
        break;
      case BAG_IF_ELSE:
        state |= IN_ALT;
        tune_called_state(NODE_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then))
          tune_called_state(en->te.Then, state);
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state(en->te.Else, state);
        break;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;

      tune_called_state(NODE_QUANT_BODY(qn), state);
    }
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      switch (an->type) {
      case ANCR_PREC_READ_NOT:
      case ANCR_LOOK_BEHIND_NOT:
        state |= IN_NOT;
        /* fall */
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        tune_called_state(NODE_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case NODE_BACKREF:
  case NODE_STRING:
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_GIMMICK:
  default:
    break;
  }
}