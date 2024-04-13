node_max_byte_len(Node* node, ScanEnv* env)
{
  OnigLen len;
  OnigLen tmax;

  len = 0;
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      tmax = node_max_byte_len(NODE_CAR(node), env);
      len = distance_add(len, tmax);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    do {
      tmax = node_max_byte_len(NODE_CAR(node), env);
      if (len < tmax) len = tmax;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);
      len = (OnigLen )(sn->end - sn->s);
    }
    break;

  case NODE_CTYPE:
  case NODE_CCLASS:
    len = ONIGENC_MBC_MAXLEN_DIST(env->enc);
    break;

  case NODE_BACKREF:
    if (! NODE_IS_CHECKER(node)) {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      if (NODE_IS_RECURSION(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          len = INFINITE_LEN;
        }
#endif
        break;
      }
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        tmax = node_max_byte_len(mem_env[backs[i]].mem_node, env);
        if (len < tmax) len = tmax;
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (! NODE_IS_RECURSION(node))
      len = node_max_byte_len(NODE_BODY(node), env);
    else
      len = INFINITE_LEN;
    break;
#endif

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->upper != 0) {
        len = node_max_byte_len(NODE_BODY(node), env);
        if (len != 0) {
          if (! IS_INFINITE_REPEAT(qn->upper))
            len = distance_multiply(len, qn->upper);
          else
            len = INFINITE_LEN;
        }
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_FIXED_MAX(node))
          len = en->max_len;
        else {
          if (NODE_IS_MARK1(node))
            len = INFINITE_LEN;
          else {
            NODE_STATUS_ADD(node, MARK1);
            len = node_max_byte_len(NODE_BODY(node), env);
            NODE_STATUS_REMOVE(node, MARK1);

            en->max_len = len;
            NODE_STATUS_ADD(node, FIXED_MAX);
          }
        }
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        len = node_max_byte_len(NODE_BODY(node), env);
        break;
      case BAG_IF_ELSE:
        {
          OnigLen tlen, elen;

          len = node_max_byte_len(NODE_BODY(node), env);
          if (IS_NOT_NULL(en->te.Then)) {
            tlen = node_max_byte_len(en->te.Then, env);
            len = distance_add(len, tlen);
          }
          if (IS_NOT_NULL(en->te.Else))
            elen = node_max_byte_len(en->te.Else, env);
          else elen = 0;

          if (elen > len) len = elen;
        }
        break;
      }
    }
    break;

  case NODE_ANCHOR:
  case NODE_GIMMICK:
  default:
    break;
  }

  return len;
}