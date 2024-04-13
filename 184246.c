tune_called_state_call(Node* node, int state)
{
  switch (NODE_TYPE(node)) {
  case NODE_ALT:
    state |= IN_ALT;
    /* fall */
  case NODE_LIST:
    do {
      tune_called_state_call(NODE_CAR(node), state);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
        state |= IN_REAL_REPEAT;
      if (qn->lower != qn->upper)
        state |= IN_VAR_REPEAT;

      tune_called_state_call(NODE_QUANT_BODY(qn), state);
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
        tune_called_state_call(NODE_ANCHOR_BODY(an), state);
        break;
      default:
        break;
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK1(node)) {
          if ((~en->m.called_state & state) != 0) {
            en->m.called_state |= state;
            tune_called_state_call(NODE_BODY(node), state);
          }
        }
        else {
          NODE_STATUS_ADD(node, MARK1);
          en->m.called_state |= state;
          tune_called_state_call(NODE_BODY(node), state);
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        state |= IN_ALT;
        tune_called_state_call(NODE_BODY(node), state);
        if (IS_NOT_NULL(en->te.Then)) {
          tune_called_state_call(en->te.Then, state);
        }
        if (IS_NOT_NULL(en->te.Else))
          tune_called_state_call(en->te.Else, state);
      }
      else {
        tune_called_state_call(NODE_BODY(node), state);
      }
    }
    break;

  case NODE_CALL:
    tune_called_state_call(NODE_BODY(node), state);
    break;

  default:
    break;
  }
}