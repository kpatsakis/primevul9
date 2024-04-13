scan_env_add_mem_entry(ScanEnv* env)
{
  int i, need, alloc;
  MemEnv* p;

  need = env->num_mem + 1;
  if (need > MaxCaptureNum && MaxCaptureNum != 0)
    return ONIGERR_TOO_MANY_CAPTURES;

  if (need >= SCANENV_MEMENV_SIZE) {
    if (env->mem_alloc <= need) {
      if (IS_NULL(env->mem_env_dynamic)) {
        alloc = INIT_SCANENV_MEMENV_ALLOC_SIZE;
        p = (MemEnv* )xmalloc(sizeof(MemEnv) * alloc);
        xmemcpy(p, env->mem_env_static, sizeof(env->mem_env_static));
      }
      else {
        alloc = env->mem_alloc * 2;
        p = (MemEnv* )xrealloc(env->mem_env_dynamic, sizeof(MemEnv) * alloc);
      }
      CHECK_NULL_RETURN_MEMERR(p);

      for (i = env->num_mem + 1; i < alloc; i++) {
        p[i].node = NULL_NODE;
#if 0
        p[i].in   = 0;
        p[i].recursion = 0;
#endif
      }

      env->mem_env_dynamic = p;
      env->mem_alloc = alloc;
    }
  }

  env->num_mem++;
  return env->num_mem;
}