flatpak_run_apply_env_default (FlatpakBwrap *bwrap, gboolean use_ld_so_cache)
{
  bwrap->envp = apply_exports (bwrap->envp, default_exports, G_N_ELEMENTS (default_exports));

  if (!use_ld_so_cache)
    bwrap->envp = apply_exports (bwrap->envp, no_ld_so_cache_exports, G_N_ELEMENTS (no_ld_so_cache_exports));
}