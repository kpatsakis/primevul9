tune_look_behind(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r;
  int state1;
  int used;
  MinMaxCharLen ci;
  Node* body;
  AnchorNode* an = ANCHOR_(node);

  used = FALSE;
  r = check_node_in_look_behind(NODE_ANCHOR_BODY(an),
                                an->type == ANCR_LOOK_BEHIND_NOT ? 1 : 0,
                                &used);
  if (r < 0) return r;
  if (r > 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;

  if (an->type == ANCR_LOOK_BEHIND_NOT)
    state1 = state | IN_NOT | IN_LOOK_BEHIND;
  else
    state1 = state | IN_LOOK_BEHIND;

  body = NODE_ANCHOR_BODY(an);
  /* Execute tune_tree(body) before call node_char_len().
     Because case-fold expansion must be done before node_char_len().
   */
  r = tune_tree(body, reg, state1, env);
  if (r != 0) return r;

  r = alt_reduce_in_look_behind(body, reg, env);
  if (r != 0) return r;

  r = node_char_len(body, reg, &ci, env);
  if (r >= 0) {
    /* #177: overflow in onigenc_step_back() */
    if ((ci.max != INFINITE_LEN && ci.max > LOOK_BEHIND_MAX_CHAR_LEN)
      || ci.min > LOOK_BEHIND_MAX_CHAR_LEN) {
      return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
    }

    if (ci.min == 0 && ci.min_is_sure != FALSE && used == FALSE) {
      if (an->type == ANCR_LOOK_BEHIND_NOT)
        r = onig_node_reset_fail(node);
      else
        r = onig_node_reset_empty(node);

      return r;
    }

    if (r == CHAR_LEN_TOP_ALT_FIXED) {
      if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND)) {
        r = divide_look_behind_alternatives(node);
        if (r == 0)
          r = tune_tree(node, reg, state, env);
      }
      else if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_VARIABLE_LEN_LOOK_BEHIND))
        goto normal;
      else
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
    }
    else { /* CHAR_LEN_NORMAL */
    normal:
      if (ci.min == INFINITE_LEN) {
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else {
        if (ci.min != ci.max &&
            ! IS_SYNTAX_BV(env->syntax, ONIG_SYN_VARIABLE_LEN_LOOK_BEHIND)) {
          r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
        }
        else {
          Node* tail;

          /* check lead_node is already set by double call after
             divide_look_behind_alternatives() */
          if (IS_NULL(an->lead_node)) {
            an->char_min_len = ci.min;
            an->char_max_len = ci.max;
            r = get_tree_tail_literal(body, &tail, reg);
            if (r == GET_VALUE_FOUND) {
              r = onig_node_copy(&(an->lead_node), tail);
              if (r != 0) return r;
            }
          }
          r = ONIG_NORMAL;
        }
      }
    }
  }

  return r;
}