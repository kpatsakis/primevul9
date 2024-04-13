calculate_ld_cache_checksum (GBytes   *app_deploy_data,
                             GBytes   *runtime_deploy_data,
                             const char *app_extensions,
                             const char *runtime_extensions)
{
  g_autoptr(GChecksum) ld_so_checksum = g_checksum_new (G_CHECKSUM_SHA256);
  if (app_deploy_data)
    g_checksum_update (ld_so_checksum, (guchar *) flatpak_deploy_data_get_commit (app_deploy_data), -1);
  g_checksum_update (ld_so_checksum, (guchar *) flatpak_deploy_data_get_commit (runtime_deploy_data), -1);
  if (app_extensions)
    g_checksum_update (ld_so_checksum, (guchar *) app_extensions, -1);
  if (runtime_extensions)
    g_checksum_update (ld_so_checksum, (guchar *) runtime_extensions, -1);

  return g_strdup (g_checksum_get_string (ld_so_checksum));
}