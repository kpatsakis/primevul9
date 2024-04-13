flatpak_ensure_data_dir (GFile        *app_id_dir,
                         GCancellable *cancellable,
                         GError      **error)
{
  g_autoptr(GFile) data_dir = g_file_get_child (app_id_dir, "data");
  g_autoptr(GFile) cache_dir = g_file_get_child (app_id_dir, "cache");
  g_autoptr(GFile) fontconfig_cache_dir = g_file_get_child (cache_dir, "fontconfig");
  g_autoptr(GFile) tmp_dir = g_file_get_child (cache_dir, "tmp");
  g_autoptr(GFile) config_dir = g_file_get_child (app_id_dir, "config");

  if (!flatpak_mkdir_p (data_dir, cancellable, error))
    return FALSE;

  if (!flatpak_mkdir_p (cache_dir, cancellable, error))
    return FALSE;

  if (!flatpak_mkdir_p (fontconfig_cache_dir, cancellable, error))
    return FALSE;

  if (!flatpak_mkdir_p (tmp_dir, cancellable, error))
    return FALSE;

  if (!flatpak_mkdir_p (config_dir, cancellable, error))
    return FALSE;

  return TRUE;
}