onig_end(void)
{
  exec_end_call_list();

#ifdef USE_CALLOUT
  onig_global_callout_names_free();
#endif

  onigenc_end();

  onig_inited = 0;

  return 0;
}