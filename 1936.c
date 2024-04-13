add_icon_path_args (FlatpakBwrap *bwrap)
{
  g_autofree gchar *user_icons_path = NULL;
  g_autoptr(GFile) user_icons = NULL;

  if (g_file_test ("/usr/share/icons", G_FILE_TEST_IS_DIR))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", "/usr/share/icons", "/run/host/share/icons",
                              NULL);
    }

  user_icons_path = g_build_filename (g_get_user_data_dir (), "icons", NULL);
  user_icons = g_file_new_for_path (user_icons_path);
  if (g_file_query_exists (user_icons, NULL))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", flatpak_file_get_path_cached (user_icons), "/run/host/user-share/icons",
                              NULL);
    }
}