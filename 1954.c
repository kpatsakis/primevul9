flatpak_run_add_journal_args (FlatpakBwrap *bwrap)
{
  g_autofree char *journal_socket_socket = g_strdup ("/run/systemd/journal/socket");
  g_autofree char *journal_stdout_socket = g_strdup ("/run/systemd/journal/stdout");

  if (g_file_test (journal_socket_socket, G_FILE_TEST_EXISTS))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", journal_socket_socket, journal_socket_socket,
                              NULL);
    }
  if (g_file_test (journal_stdout_socket, G_FILE_TEST_EXISTS))
    {
      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", journal_stdout_socket, journal_stdout_socket,
                              NULL);
    }
}