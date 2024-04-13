node_new_backref(int back_num, int* backrefs, int by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                 int exist_level, int nest_level,
#endif
                 ScanEnv* env)
{
  int i;
  Node* node = node_new();

  CHECK_NULL_RETURN(node);

  NODE_SET_TYPE(node, NODE_BACKREF);
  BACKREF_(node)->back_num = back_num;
  BACKREF_(node)->back_dynamic = (int* )NULL;
  if (by_name != 0)
    NODE_STATUS_ADD(node, NST_BY_NAME);

#ifdef USE_BACKREF_WITH_LEVEL
  if (exist_level != 0) {
    NODE_STATUS_ADD(node, NST_NEST_LEVEL);
    BACKREF_(node)->nest_level  = nest_level;
  }
#endif

  for (i = 0; i < back_num; i++) {
    if (backrefs[i] <= env->num_mem &&
        IS_NULL(SCANENV_MEMENV(env)[backrefs[i]].node)) {
      NODE_STATUS_ADD(node, NST_RECURSION);   /* /...(\1).../ */
      break;
    }
  }

  if (back_num <= NODE_BACKREFS_SIZE) {
    for (i = 0; i < back_num; i++)
      BACKREF_(node)->back_static[i] = backrefs[i];
  }
  else {
    int* p = (int* )xmalloc(sizeof(int) * back_num);
    if (IS_NULL(p)) {
      onig_node_free(node);
      return NULL;
    }
    BACKREF_(node)->back_dynamic = p;
    for (i = 0; i < back_num; i++)
      p[i] = backrefs[i];
  }
  return node;
}