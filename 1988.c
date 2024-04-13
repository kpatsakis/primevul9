add_font_path_args (FlatpakBwrap *bwrap)
{
  g_autoptr(GString) xml_snippet = g_string_new ("");
  gchar *path_build_tmp = NULL;
  g_autoptr(GFile) user_font1 = NULL;
  g_autoptr(GFile) user_font2 = NULL;
  g_autoptr(GFile) user_font_cache = NULL;
  g_auto(GStrv) system_cache_dirs = NULL;
  gboolean found_cache = FALSE;
  int i;


  g_string_append (xml_snippet,
                   "<?xml version=\"1.0\"?>\n"
                   "<!DOCTYPE fontconfig SYSTEM \"fonts.dtd\">\n"
                   "<fontconfig>\n");

  if (g_file_test (SYSTEM_FONTS_DIR, G_FILE_TEST_EXISTS))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", SYSTEM_FONTS_DIR, "/run/host/fonts",
                              NULL);
      g_string_append_printf (xml_snippet,
                              "\t<remap-dir as-path=\"%s\">/run/host/fonts</remap-dir>\n",
                              SYSTEM_FONTS_DIR);
    }

  if (g_file_test ("/usr/local/share/fonts", G_FILE_TEST_EXISTS))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", "/usr/local/share/fonts", "/run/host/local-fonts",
                              NULL);
      g_string_append_printf (xml_snippet,
                              "\t<remap-dir as-path=\"%s\">/run/host/local-fonts</remap-dir>\n",
                              "/usr/local/share/fonts");
    }

  system_cache_dirs = g_strsplit (SYSTEM_FONT_CACHE_DIRS, ":", 0);
  for (i = 0; system_cache_dirs[i] != NULL; i++)
    {
      if (g_file_test (system_cache_dirs[i], G_FILE_TEST_EXISTS))
        {
          flatpak_bwrap_add_args (bwrap,
                                  "--ro-bind", system_cache_dirs[i], "/run/host/fonts-cache",
                                  NULL);
          found_cache = TRUE;
          break;
        }
    }

  if (!found_cache)
    {
      /* We ensure these directories are never writable, or fontconfig
         will use them to write the default cache */
      flatpak_bwrap_add_args (bwrap,
                              "--tmpfs", "/run/host/fonts-cache",
                              "--remount-ro", "/run/host/fonts-cache",
                              NULL);
    }

  path_build_tmp = g_build_filename (g_get_user_data_dir (), "fonts", NULL);
  user_font1 = g_file_new_for_path (path_build_tmp);
  g_clear_pointer (&path_build_tmp, g_free);

  path_build_tmp = g_build_filename (g_get_home_dir (), ".fonts", NULL);
  user_font2 = g_file_new_for_path (path_build_tmp);
  g_clear_pointer (&path_build_tmp, g_free);

  if (g_file_query_exists (user_font1, NULL))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", flatpak_file_get_path_cached (user_font1), "/run/host/user-fonts",
                              NULL);
      g_string_append_printf (xml_snippet,
                              "\t<remap-dir as-path=\"%s\">/run/host/user-fonts</remap-dir>\n",
                              flatpak_file_get_path_cached (user_font1));
    }
  else if (g_file_query_exists (user_font2, NULL))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", flatpak_file_get_path_cached (user_font2), "/run/host/user-fonts",
                              NULL);
      g_string_append_printf (xml_snippet,
                              "\t<remap-dir as-path=\"%s\">/run/host/user-fonts</remap-dir>\n",
                              flatpak_file_get_path_cached (user_font2));
    }

  path_build_tmp = g_build_filename (g_get_user_cache_dir (), "fontconfig", NULL);
  user_font_cache = g_file_new_for_path (path_build_tmp);
  g_clear_pointer (&path_build_tmp, g_free);

  if (g_file_query_exists (user_font_cache, NULL))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", flatpak_file_get_path_cached (user_font_cache), "/run/host/user-fonts-cache",
                              NULL);
    }
  else
    {
      /* We ensure these directories are never writable, or fontconfig
         will use them to write the default cache */
      flatpak_bwrap_add_args (bwrap,
                              "--tmpfs", "/run/host/user-fonts-cache",
                              "--remount-ro", "/run/host/user-fonts-cache",
                              NULL);
    }

  g_string_append (xml_snippet,
                   "</fontconfig>\n");

  if (!flatpak_bwrap_add_args_data (bwrap, "font-dirs.xml", xml_snippet->str, xml_snippet->len, "/run/host/font-dirs.xml", NULL))
    g_warning ("Unable to add fontconfig data snippet");
}