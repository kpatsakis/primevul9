scan_env_clear(ScanEnv* env)
{
  BIT_STATUS_CLEAR(env->capture_history);
  BIT_STATUS_CLEAR(env->bt_mem_start);
  BIT_STATUS_CLEAR(env->bt_mem_end);
  BIT_STATUS_CLEAR(env->backrefed_mem);
  env->error      = (UChar* )NULL;
  env->error_end  = (UChar* )NULL;
  env->num_call   = 0;
  env->num_mem    = 0;
#ifdef USE_NAMED_GROUP
  env->num_named  = 0;
#endif
  env->mem_alloc       = 0;
  env->mem_env_dynamic = (MemEnv* )NULL;

  xmemset(env->mem_env_static, 0, sizeof(env->mem_env_static));

#ifdef USE_COMBINATION_EXPLOSION_CHECK
  env->num_comb_exp_check  = 0;
  env->comb_exp_max_regnum = 0;
  env->curr_max_regnum     = 0;
  env->has_recursion       = 0;
#endif
  env->parse_depth         = 0;
}