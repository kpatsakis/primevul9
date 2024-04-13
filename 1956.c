flatpak_run_add_resolved_args (FlatpakBwrap *bwrap)
{
  const char *resolved_socket = "/run/systemd/resolve/io.systemd.Resolve";

  if (g_file_test (resolved_socket, G_FILE_TEST_EXISTS))
    flatpak_bwrap_add_args (bwrap, "--bind", resolved_socket, resolved_socket, NULL);
}