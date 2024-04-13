tree_min_len(Node* node, ScanEnv* env)
{
  OnigLen len;
  OnigLen tmin;

  len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_BACKREF:
    if (! NODE_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (NODE_IS_RECURSION(node)) break;

      backs = BACKREFS_P(br);
      len = tree_min_len(mem_env[backs[0]].node, env);
      for (i = 1; i < br->back_num; i++) {
        tmin = tree_min_len(mem_env[backs[i]].node, env);
        if (len > tmin) len = tmin;
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    {
      Node* t = NODE_BODY(node);
      if (NODE_IS_RECURSION(node)) {
        if (NODE_IS_MIN_FIXED(t))
          len = ENCLOSURE_(t)->min_len;
      }
      else
        len = tree_min_len(t, env);
    }
    break;
#endif

  case NODE_LIST:
    do {
      tmin = tree_min_len(NODE_CAR(node), env);
      len = distance_add(len, tmin);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      Node *x, *y;
      y = node;
      do {
        x = NODE_CAR(y);
        tmin = tree_min_len(x, env);
        if (y == node) len = tmin;
        else if (len > tmin) len = tmin;
      } while (IS_NOT_NULL(y = NODE_CDR(y)));
    }
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      len = (int )(sn->end - sn->s);
    }
    break;

  case NODE_CTYPE:
  case NODE_CCLASS:
    len = ONIGENC_MBC_MINLEN(env->enc);
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->lower > 0) {
        len = tree_min_len(NODE_BODY(node), env);
        len = distance_multiply(len, qn->lower);
      }
    }
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);
      switch (en->type) {
      case ENCLOSURE_MEMORY:
        if (NODE_IS_MIN_FIXED(node))
          len = en->min_len;
        else {
          if (NODE_IS_MARK1(node))
            len = 0;  // recursive
          else {
            NODE_STATUS_ADD(node, NST_MARK1);
            len = tree_min_len(NODE_BODY(node), env);
            NODE_STATUS_REMOVE(node, NST_MARK1);

            en->min_len = len;
            NODE_STATUS_ADD(node, NST_MIN_FIXED);
          }
        }
        break;

      case ENCLOSURE_OPTION:
      case ENCLOSURE_STOP_BACKTRACK:
        len = tree_min_len(NODE_BODY(node), env);
        break;
      case ENCLOSURE_IF_ELSE:
        {
          OnigLen elen;

          len = tree_min_len(NODE_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then))
            len += tree_min_len(en->te.Then, env);
          if (IS_NOT_NULL(en->te.Else))
            elen = tree_min_len(en->te.Else, env);
          else elen = 0;

          if (elen < len) len = elen;
        }
        break;
      }
    }
    break;

  case NODE_GIMMICK:
    {
      GimmickNode* g = GIMMICK_(node);
      if (g->type == GIMMICK_FAIL) {
        len = INFINITE_LEN;
        break;
      }
    }
    /* fall */
  case NODE_ANCHOR:
  default:
    break;
  }

  return len;
}