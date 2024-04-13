flatpak_run_add_pcsc_args (FlatpakBwrap *bwrap)
{
  const char * pcsc_socket;
  const char * sandbox_pcsc_socket = "/run/pcscd/pcscd.comm";

  pcsc_socket = g_getenv ("PCSCLITE_CSOCK_NAME");
  if (pcsc_socket)
    {
      if (!g_file_test (pcsc_socket, G_FILE_TEST_EXISTS))
        {
          flatpak_bwrap_unset_env (bwrap, "PCSCLITE_CSOCK_NAME");
          return;
        }
    }
  else
    {
      pcsc_socket = "/run/pcscd/pcscd.comm";
      if (!g_file_test (pcsc_socket, G_FILE_TEST_EXISTS))
        return;
    }

  flatpak_bwrap_add_args (bwrap,
                          "--ro-bind", pcsc_socket, sandbox_pcsc_socket,
                          NULL);
  flatpak_bwrap_set_env (bwrap, "PCSCLITE_CSOCK_NAME", sandbox_pcsc_socket, TRUE);
}