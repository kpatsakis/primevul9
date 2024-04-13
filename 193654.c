check_backrefs(Node* node, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_backrefs(NODE_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = check_backrefs(NODE_BODY(node), env);
    break;

  case NODE_ENCLOSURE:
    r = check_backrefs(NODE_BODY(node), env);
    {
      EnclosureNode* en = ENCLOSURE_(node);

      if (en->type == ENCLOSURE_IF_ELSE) {
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_backrefs(en->te.Then, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_backrefs(en->te.Else, env);
        }
      }
    }
    break;

  case NODE_BACKREF:
    {
      int i;
      BackRefNode* br = BACKREF_(node);
      int* backs = BACKREFS_P(br);
      MemEnv* mem_env = SCANENV_MEMENV(env);

      for (i = 0; i < br->back_num; i++) {
        if (backs[i] > env->num_mem)
          return ONIGERR_INVALID_BACKREF;

        NODE_STATUS_ADD(mem_env[backs[i]].node, NST_BACKREF);
      }
      r = 0;
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}