tune_anchor(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r;
  AnchorNode* an = ANCHOR_(node);

  switch (an->type) {
  case ANCR_PREC_READ:
    r = tune_tree(NODE_ANCHOR_BODY(an), reg, state, env);
    break;
  case ANCR_PREC_READ_NOT:
    r = tune_tree(NODE_ANCHOR_BODY(an), reg, (state | IN_NOT), env);
    break;

  case ANCR_LOOK_BEHIND:
  case ANCR_LOOK_BEHIND_NOT:
    r = tune_look_behind(node, reg, state, env);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}