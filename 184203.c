node_char_len1(Node* node, regex_t* reg, MinMaxCharLen* ci, ScanEnv* env,
               int level)
{
  MinMaxCharLen tci;
  int r = CHAR_LEN_NORMAL;

  level++;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      int first = TRUE;
      do {
        r = node_char_len1(NODE_CAR(node), reg, &tci, env, level);
        if (r < 0) break;
        if (first == TRUE) {
          *ci = tci;
          first = FALSE;
        }
        else
          mmcl_add(ci, &tci);
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_ALT:
    {
      int fixed;

      r = node_char_len1(NODE_CAR(node), reg, ci, env, level);
      if (r < 0) break;

      fixed = TRUE;
      while (IS_NOT_NULL(node = NODE_CDR(node))) {
        r = node_char_len1(NODE_CAR(node), reg, &tci, env, level);
        if (r < 0) break;
        if (! mmcl_fixed(&tci))
          fixed = FALSE;
        mmcl_alt_merge(ci, &tci);
      }
      if (r < 0) break;

      r = CHAR_LEN_NORMAL;
      if (mmcl_fixed(ci)) break;

      if (fixed == TRUE && level == 1) {
        r = CHAR_LEN_TOP_ALT_FIXED;
      }
    }
    break;

  case NODE_STRING:
    {
      OnigLen clen;
      StrNode* sn = STR_(node);
      UChar *s = sn->s;

      if (NODE_IS_IGNORECASE(node) && ! NODE_STRING_IS_CRUDE(node)) {
        /* Such a case is possible.
           ex. /(?i)(?<=\1)(a)/
           Backref node refer to capture group, but it doesn't tune yet.
         */
        r = ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
        break;
      }

      clen = 0;
      while (s < sn->end) {
        s += enclen(reg->enc, s);
        clen = distance_add(clen, 1);
      }
      mmcl_set(ci, clen);
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower == qn->upper) {
        if (qn->upper == 0) {
          mmcl_set(ci, 0);
        }
        else {
          r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
          if (r < 0) break;
          mmcl_multiply(ci, qn->lower);
        }
      }
      else {
        r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
        if (r < 0) break;
        mmcl_repeat_range_multiply(ci, qn->lower, qn->upper);
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (NODE_IS_RECURSION(node))
      mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
    else
      r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
    break;
#endif

  case NODE_CTYPE:
  case NODE_CCLASS:
    mmcl_set(ci, 1);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_FIXED_CLEN(node)) {
          mmcl_set_min_max(ci, en->min_char_len, en->max_char_len,
                           NODE_IS_FIXED_CLEN_MIN_SURE(node));
        }
        else {
          if (NODE_IS_MARK1(node)) {
            mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
          }
          else {
            NODE_STATUS_ADD(node, MARK1);
            r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
            NODE_STATUS_REMOVE(node, MARK1);
            if (r < 0) break;

            en->min_char_len = ci->min;
            en->max_char_len = ci->max;
            NODE_STATUS_ADD(node, FIXED_CLEN);
            if (ci->min_is_sure != FALSE)
              NODE_STATUS_ADD(node, FIXED_CLEN_MIN_SURE);
          }
        }
        /* can't optimize look-behind if capture exists. */
        ci->min_is_sure = FALSE;
        break;
      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
        break;
      case BAG_IF_ELSE:
        {
          MinMaxCharLen eci;

          r = node_char_len1(NODE_BODY(node), reg, ci, env, level);
          if (r < 0) break;

          if (IS_NOT_NULL(en->te.Then)) {
            r = node_char_len1(en->te.Then, reg, &tci, env, level);
            if (r < 0) break;
            mmcl_add(ci, &tci);
          }

          if (IS_NOT_NULL(en->te.Else)) {
            r = node_char_len1(en->te.Else, reg, &eci, env, level);
            if (r < 0) break;
          }
          else {
            mmcl_set(&eci, 0);
          }

          mmcl_alt_merge(ci, &eci);
        }
        break;
      default: /* never come here */
        r = ONIGERR_PARSER_BUG;
        break;
      }
    }
    break;

  case NODE_GIMMICK:
    mmcl_set(ci, 0);
    break;

  case NODE_ANCHOR:
  zero:
    mmcl_set(ci, 0);
    /* can't optimize look-behind if anchor exists. */
    ci->min_is_sure = FALSE;
    break;

  case NODE_BACKREF:
    if (NODE_IS_CHECKER(node))
      goto zero;

    if (NODE_IS_RECURSION(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
      if (NODE_IS_NEST_LEVEL(node)) {
        mmcl_set_min_max(ci, 0, INFINITE_LEN, FALSE);
        break;
      }
#endif

      mmcl_set_min_max(ci, 0, 0, FALSE);
      break;
    }

    {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);

      backs = BACKREFS_P(br);
      r = node_char_len1(mem_env[backs[0]].mem_node, reg, ci, env, level);
      if (r < 0) break;
      if (! mmcl_fixed(ci)) ci->min_is_sure = FALSE;

      for (i = 1; i < br->back_num; i++) {
        r = node_char_len1(mem_env[backs[i]].mem_node, reg, &tci, env, level);
        if (r < 0) break;
        if (! mmcl_fixed(&tci)) tci.min_is_sure = FALSE;
        mmcl_alt_merge(ci, &tci);
      }
    }
    break;

  default: /* never come here */
    r = ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}