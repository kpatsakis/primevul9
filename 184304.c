compile_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int r = 0;

  switch (node->type) {
  case GIMMICK_FAIL:
    r = add_op(reg, OP_FAIL);
    break;

  case GIMMICK_SAVE:
    r = add_op(reg, OP_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->save_val.type = node->detail_type;
    COP(reg)->save_val.id   = node->id;
    break;

  case GIMMICK_UPDATE_VAR:
    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = node->detail_type;
    COP(reg)->update_var.id   = node->id;
    COP(reg)->update_var.clear = FALSE;
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
    case ONIG_CALLOUT_OF_NAME:
      {
        if (node->detail_type == ONIG_CALLOUT_OF_NAME) {
          r = add_op(reg, OP_CALLOUT_NAME);
          if (r != 0) return r;
          COP(reg)->callout_name.id  = node->id;
          COP(reg)->callout_name.num = node->num;
        }
        else {
          r = add_op(reg, OP_CALLOUT_CONTENTS);
          if (r != 0) return r;
          COP(reg)->callout_contents.num = node->num;
        }
      }
      break;

    default:
      r = ONIGERR_TYPE_BUG;
      break;
    }
#endif
  }

  return r;
}