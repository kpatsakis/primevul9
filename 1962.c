flatpak_run_add_ssh_args (FlatpakBwrap *bwrap)
{
  static const char sandbox_auth_socket[] = "/run/flatpak/ssh-auth";
  const char * auth_socket;

  auth_socket = g_getenv ("SSH_AUTH_SOCK");

  if (!auth_socket)
    return; /* ssh agent not present */

  if (!g_file_test (auth_socket, G_FILE_TEST_EXISTS))
    {
      /* Let's clean it up, so that the application will not try to connect */
      flatpak_bwrap_unset_env (bwrap, "SSH_AUTH_SOCK");
      return;
    }

  flatpak_bwrap_add_args (bwrap,
                          "--ro-bind", auth_socket, sandbox_auth_socket,
                          NULL);
  flatpak_bwrap_set_env (bwrap, "SSH_AUTH_SOCK", sandbox_auth_socket, TRUE);
}