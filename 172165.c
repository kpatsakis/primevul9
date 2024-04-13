node_new_backref_checker(int back_num, int* backrefs, int by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                         int exist_level, int nest_level,
#endif
                         ScanEnv* env)
{
  Node* node;

  node = node_new_backref(back_num, backrefs, by_name,
#ifdef USE_BACKREF_WITH_LEVEL
                          exist_level, nest_level,
#endif
                          env);
  CHECK_NULL_RETURN(node);

  NODE_STATUS_ADD(node, NST_CHECKER);
  return node;
}