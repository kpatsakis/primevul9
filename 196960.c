scan_env_add_mem_entry(ScanEnv* env)
{
  int i, need, alloc;
  Node** p;

  need = env->num_mem + 1;
  if (need > ONIG_MAX_CAPTURE_NUM)
    return ONIGERR_TOO_MANY_CAPTURES;

  if (need >= SCANENV_MEMNODES_SIZE) {
    if (env->mem_alloc <= need) {
      if (IS_NULL(env->mem_nodes_dynamic)) {
        alloc = INIT_SCANENV_MEMNODES_ALLOC_SIZE;
        p = (Node** )xmalloc(sizeof(Node*) * alloc);
        xmemcpy(p, env->mem_nodes_static,
                sizeof(Node*) * SCANENV_MEMNODES_SIZE);
      }
      else {
        alloc = env->mem_alloc * 2;
        p = (Node** )xrealloc(env->mem_nodes_dynamic, sizeof(Node*) * alloc);
      }
      CHECK_NULL_RETURN_MEMERR(p);

      for (i = env->num_mem + 1; i < alloc; i++)
        p[i] = NULL_NODE;

      env->mem_nodes_dynamic = p;
      env->mem_alloc = alloc;
    }
  }

  env->num_mem++;
  return env->num_mem;
}