flatpak_run_apply_env_prompt (FlatpakBwrap *bwrap, const char *app_id)
{
  /* A custom shell prompt. FLATPAK_ID is always set.
   * PS1 can be overwritten by runtime metadata or by --env overrides
   */
  flatpak_bwrap_set_env (bwrap, "FLATPAK_ID", app_id, TRUE);
  flatpak_bwrap_set_env (bwrap, "PS1", "[📦 $FLATPAK_ID \\W]\\$ ", FALSE);
}