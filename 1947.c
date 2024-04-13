flatpak_run_extend_ld_path (FlatpakBwrap *bwrap,
                            const char *prepend,
                            const char *append)
{
  g_autoptr(GString) ld_library_path = g_string_new (g_environ_getenv (bwrap->envp, "LD_LIBRARY_PATH"));

  if (prepend != NULL && *prepend != '\0')
    {
      if (ld_library_path->len > 0)
        g_string_prepend (ld_library_path, ":");

      g_string_prepend (ld_library_path, prepend);
    }

  if (append != NULL && *append != '\0')
    {
      if (ld_library_path->len > 0)
        g_string_append (ld_library_path, ":");

      g_string_append (ld_library_path, append);
    }

  flatpak_bwrap_set_env (bwrap, "LD_LIBRARY_PATH", ld_library_path->str, TRUE);
}