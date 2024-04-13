add_monitor_path_args (gboolean      use_session_helper,
                       FlatpakBwrap *bwrap)
{
  g_autoptr(AutoFlatpakSessionHelper) session_helper = NULL;
  g_autofree char *monitor_path = NULL;
  g_autofree char *pkcs11_socket_path = NULL;
  g_autoptr(GVariant) session_data = NULL;

  if (use_session_helper)
    {
      session_helper =
        flatpak_session_helper_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS,
                                                       FLATPAK_SESSION_HELPER_BUS_NAME,
                                                       FLATPAK_SESSION_HELPER_PATH,
                                                       NULL, NULL);
    }

  if (session_helper &&
      flatpak_session_helper_call_request_session_sync (session_helper,
                                                        &session_data,
                                                        NULL, NULL))
    {
      if (g_variant_lookup (session_data, "path", "s", &monitor_path))
        flatpak_bwrap_add_args (bwrap,
                                "--ro-bind", monitor_path, "/run/host/monitor",
                                "--symlink", "/run/host/monitor/resolv.conf", "/etc/resolv.conf",
                                "--symlink", "/run/host/monitor/host.conf", "/etc/host.conf",
                                "--symlink", "/run/host/monitor/hosts", "/etc/hosts",
                                "--symlink", "/run/host/monitor/gai.conf", "/etc/gai.conf",
                                NULL);

      if (g_variant_lookup (session_data, "pkcs11-socket", "s", &pkcs11_socket_path))
        {
          static const char sandbox_pkcs11_socket_path[] = "/run/flatpak/p11-kit/pkcs11";
          const char *trusted_module_contents =
            "# This overrides the runtime p11-kit-trusted module with a client one talking to the trust module on the host\n"
            "module: p11-kit-client.so\n";

          if (flatpak_bwrap_add_args_data (bwrap, "p11-kit-trust.module",
                                           trusted_module_contents, -1,
                                           "/etc/pkcs11/modules/p11-kit-trust.module", NULL))
            {
              flatpak_bwrap_add_args (bwrap,
                                      "--ro-bind", pkcs11_socket_path, sandbox_pkcs11_socket_path,
                                      NULL);
              flatpak_bwrap_unset_env (bwrap, "P11_KIT_SERVER_ADDRESS");
              flatpak_bwrap_add_runtime_dir_member (bwrap, "p11-kit");
            }
        }
    }
  else
    {
      if (g_file_test ("/etc/resolv.conf", G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_args (bwrap,
                                "--ro-bind", "/etc/resolv.conf", "/etc/resolv.conf",
                                NULL);
      if (g_file_test ("/etc/host.conf", G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_args (bwrap,
                                "--ro-bind", "/etc/host.conf", "/etc/host.conf",
                                NULL);
      if (g_file_test ("/etc/hosts", G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_args (bwrap,
                                "--ro-bind", "/etc/hosts", "/etc/hosts",
                                NULL);
      if (g_file_test ("/etc/gai.conf", G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_args (bwrap,
                                "--ro-bind", "/etc/gai.conf", "/etc/gai.conf",
                                NULL);
    }
}