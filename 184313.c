set_empty_status_check_trav(Node* node, ScanEnv* env)
{
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      set_empty_status_check_trav(NODE_CAR(node), env);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      if (! ANCHOR_HAS_BODY(an)) break;
      set_empty_status_check_trav(NODE_BODY(node), env);
    }
    break;

  case NODE_QUANT:
    set_empty_status_check_trav(NODE_BODY(node), env);
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node)))
      set_empty_status_check_trav(NODE_BODY(node), env);
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          set_empty_status_check_trav(en->te.Then, env);
        }
        if (IS_NOT_NULL(en->te.Else)) {
          set_empty_status_check_trav(en->te.Else, env);
        }
      }
    }
    break;

  case NODE_BACKREF:
    {
      int i;
      int* backs;
      MemEnv* mem_env = SCANENV_MEMENV(env);
      BackRefNode* br = BACKREF_(node);
      backs = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        Node* ernode = mem_env[backs[i]].empty_repeat_node;
        if (IS_NOT_NULL(ernode)) {
          if (! is_ancestor_node(ernode, node)) {
            MEM_STATUS_LIMIT_ON(env->reg->empty_status_mem, backs[i]);
            NODE_STATUS_ADD(ernode, EMPTY_STATUS_CHECK);
            NODE_STATUS_ADD(mem_env[backs[i]].mem_node, EMPTY_STATUS_CHECK);
          }
        }
      }
    }
    break;

  default:
    break;
  }
}