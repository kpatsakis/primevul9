flatpak_run_apply_env_appid (FlatpakBwrap *bwrap,
                             GFile        *app_dir)
{
  g_autoptr(GFile) app_dir_data = NULL;
  g_autoptr(GFile) app_dir_config = NULL;
  g_autoptr(GFile) app_dir_cache = NULL;

  app_dir_data = g_file_get_child (app_dir, "data");
  app_dir_config = g_file_get_child (app_dir, "config");
  app_dir_cache = g_file_get_child (app_dir, "cache");
  flatpak_bwrap_set_env (bwrap, "XDG_DATA_HOME", flatpak_file_get_path_cached (app_dir_data), TRUE);
  flatpak_bwrap_set_env (bwrap, "XDG_CONFIG_HOME", flatpak_file_get_path_cached (app_dir_config), TRUE);
  flatpak_bwrap_set_env (bwrap, "XDG_CACHE_HOME", flatpak_file_get_path_cached (app_dir_cache), TRUE);

  if (g_getenv ("XDG_DATA_HOME"))
    flatpak_bwrap_set_env (bwrap, "HOST_XDG_DATA_HOME", g_getenv ("XDG_DATA_HOME"), TRUE);
  if (g_getenv ("XDG_CONFIG_HOME"))
    flatpak_bwrap_set_env (bwrap, "HOST_XDG_CONFIG_HOME", g_getenv ("XDG_CONFIG_HOME"), TRUE);
  if (g_getenv ("XDG_CACHE_HOME"))
    flatpak_bwrap_set_env (bwrap, "HOST_XDG_CACHE_HOME", g_getenv ("XDG_CACHE_HOME"), TRUE);
}