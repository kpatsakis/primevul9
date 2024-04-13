flatpak_context_load_for_deploy (FlatpakDeploy *deploy,
                                 GError       **error)
{
  g_autoptr(FlatpakContext) context = NULL;
  g_autoptr(FlatpakContext) overrides = NULL;
  g_autoptr(GKeyFile) metakey = NULL;

  metakey = flatpak_deploy_get_metadata (deploy);
  context = flatpak_app_compute_permissions (metakey, NULL, error);
  if (context == NULL)
    return NULL;

  overrides = flatpak_deploy_get_overrides (deploy);
  flatpak_context_merge (context, overrides);

  return g_steal_pointer (&context);
}