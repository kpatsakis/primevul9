start_dbus_proxy (FlatpakBwrap *app_bwrap,
                  FlatpakBwrap *proxy_arg_bwrap,
                  const char   *app_info_path,
                  GError      **error)
{
  char x = 'x';
  const char *proxy;
  g_autofree char *commandline = NULL;
  g_autoptr(FlatpakBwrap) proxy_bwrap = NULL;
  int sync_fds[2] = {-1, -1};
  int proxy_start_index;

  proxy_bwrap = flatpak_bwrap_new (NULL);

  if (!add_bwrap_wrapper (proxy_bwrap, app_info_path, error))
    return FALSE;

  proxy = g_getenv ("FLATPAK_DBUSPROXY");
  if (proxy == NULL)
    proxy = DBUSPROXY;

  flatpak_bwrap_add_arg (proxy_bwrap, proxy);

  proxy_start_index = proxy_bwrap->argv->len;

  if (pipe2 (sync_fds, O_CLOEXEC) < 0)
    {
      g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                           _("Unable to create sync pipe"));
      return FALSE;
    }

  /* read end goes to app */
  flatpak_bwrap_add_args_data_fd (app_bwrap, "--sync-fd", sync_fds[0], NULL);

  /* write end goes to proxy */
  flatpak_bwrap_add_fd (proxy_bwrap, sync_fds[1]);
  flatpak_bwrap_add_arg_printf (proxy_bwrap, "--fd=%d", sync_fds[1]);

  /* Note: This steals the fds from proxy_arg_bwrap */
  flatpak_bwrap_append_bwrap (proxy_bwrap, proxy_arg_bwrap);

  if (!flatpak_bwrap_bundle_args (proxy_bwrap, proxy_start_index, -1, TRUE, error))
    return FALSE;

  flatpak_bwrap_finish (proxy_bwrap);

  commandline = flatpak_quote_argv ((const char **) proxy_bwrap->argv->pdata, -1);
  g_debug ("Running '%s'", commandline);

  /* We use LEAVE_DESCRIPTORS_OPEN to work around dead-lock, see flatpak_close_fds_workaround */
  if (!g_spawn_async (NULL,
                      (char **) proxy_bwrap->argv->pdata,
                      NULL,
                      G_SPAWN_SEARCH_PATH | G_SPAWN_LEAVE_DESCRIPTORS_OPEN,
                      flatpak_bwrap_child_setup_cb, proxy_bwrap->fds,
                      NULL, error))
    return FALSE;

  /* The write end can be closed now, otherwise the read below will hang of xdg-dbus-proxy
     fails to start. */
  g_clear_pointer (&proxy_bwrap, flatpak_bwrap_free);

  /* Sync with proxy, i.e. wait until its listening on the sockets */
  if (read (sync_fds[0], &x, 1) != 1)
    {
      g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                           _("Failed to sync with dbus proxy"));
      return FALSE;
    }

  return TRUE;
}