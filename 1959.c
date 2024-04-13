create_proxy_socket (char *template)
{
  g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();
  g_autofree char *proxy_socket_dir = g_build_filename (user_runtime_dir, ".dbus-proxy", NULL);
  g_autofree char *proxy_socket = g_build_filename (proxy_socket_dir, template, NULL);
  int fd;

  if (!glnx_shutil_mkdir_p_at (AT_FDCWD, proxy_socket_dir, 0755, NULL, NULL))
    return NULL;

  fd = g_mkstemp (proxy_socket);
  if (fd == -1)
    return NULL;

  close (fd);

  return g_steal_pointer (&proxy_socket);
}