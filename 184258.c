compile_length_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int len;

  switch (node->type) {
  case GIMMICK_FAIL:
    len = OPSIZE_FAIL;
    break;

  case GIMMICK_SAVE:
    len = OPSIZE_SAVE_VAL;
    break;

  case GIMMICK_UPDATE_VAR:
    len = OPSIZE_UPDATE_VAR;
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
      len = OPSIZE_CALLOUT_CONTENTS;
      break;
    case ONIG_CALLOUT_OF_NAME:
      len = OPSIZE_CALLOUT_NAME;
      break;

    default:
      len = ONIGERR_TYPE_BUG;
      break;
    }
    break;
#endif
  }

  return len;
}