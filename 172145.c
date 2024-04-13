scan_env_clear(ScanEnv* env)
{
  MEM_STATUS_CLEAR(env->capture_history);
  MEM_STATUS_CLEAR(env->bt_mem_start);
  MEM_STATUS_CLEAR(env->bt_mem_end);
  MEM_STATUS_CLEAR(env->backrefed_mem);
  env->error      = (UChar* )NULL;
  env->error_end  = (UChar* )NULL;
  env->num_call   = 0;

#ifdef USE_CALL
  env->unset_addr_list = NULL;
  env->has_call_zero   = 0;
#endif

  env->num_mem    = 0;
  env->num_named  = 0;
  env->mem_alloc  = 0;
  env->mem_env_dynamic = (MemEnv* )NULL;

  xmemset(env->mem_env_static, 0, sizeof(env->mem_env_static));

  env->parse_depth         = 0;
  env->keep_num            = 0;
  env->save_num            = 0;
  env->save_alloc_num      = 0;
  env->saves               = 0;
}