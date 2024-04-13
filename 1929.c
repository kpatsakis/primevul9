flatpak_run_add_wayland_args (FlatpakBwrap *bwrap)
{
  const char *wayland_display;
  g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();
  g_autofree char *wayland_socket = NULL;
  g_autofree char *sandbox_wayland_socket = NULL;
  gboolean res = FALSE;
  struct stat statbuf;

  wayland_display = g_getenv ("WAYLAND_DISPLAY");
  if (!wayland_display)
    wayland_display = "wayland-0";

  wayland_socket = g_build_filename (user_runtime_dir, wayland_display, NULL);

  if (!g_str_has_prefix (wayland_display, "wayland-") ||
      strchr (wayland_display, '/') != NULL)
    {
      wayland_display = "wayland-0";
      flatpak_bwrap_set_env (bwrap, "WAYLAND_DISPLAY", wayland_display, TRUE);
    }

  sandbox_wayland_socket = g_strdup_printf ("/run/flatpak/%s", wayland_display);

  if (stat (wayland_socket, &statbuf) == 0 &&
      (statbuf.st_mode & S_IFMT) == S_IFSOCK)
    {
      res = TRUE;
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", wayland_socket, sandbox_wayland_socket,
                              NULL);
      flatpak_bwrap_add_runtime_dir_member (bwrap, wayland_display);
    }
  return res;
}