add_document_portal_args (FlatpakBwrap *bwrap,
                          const char   *app_id,
                          char        **out_mount_path)
{
  g_autoptr(GDBusConnection) session_bus = NULL;
  g_autofree char *doc_mount_path = NULL;

  session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  if (session_bus)
    {
      g_autoptr(GError) local_error = NULL;
      g_autoptr(GDBusMessage) reply = NULL;
      g_autoptr(GDBusMessage) msg =
        g_dbus_message_new_method_call ("org.freedesktop.portal.Documents",
                                        "/org/freedesktop/portal/documents",
                                        "org.freedesktop.portal.Documents",
                                        "GetMountPoint");
      g_dbus_message_set_body (msg, g_variant_new ("()"));
      reply =
        g_dbus_connection_send_message_with_reply_sync (session_bus, msg,
                                                        G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                        30000,
                                                        NULL,
                                                        NULL,
                                                        NULL);
      if (reply)
        {
          if (g_dbus_message_to_gerror (reply, &local_error))
            {
              if (g_error_matches (local_error, G_DBUS_ERROR, G_DBUS_ERROR_SERVICE_UNKNOWN))
                g_debug ("Document portal not available, not mounting /run/flatpak/doc");
              else
                g_message ("Can't get document portal: %s", local_error->message);
            }
          else
            {
              static const char dst_path[] = "/run/flatpak/doc";
              g_autofree char *src_path = NULL;
              g_variant_get (g_dbus_message_get_body (reply),
                             "(^ay)", &doc_mount_path);

              src_path = g_strdup_printf ("%s/by-app/%s",
                                          doc_mount_path, app_id);
              flatpak_bwrap_add_args (bwrap, "--bind", src_path, dst_path, NULL);
              flatpak_bwrap_add_runtime_dir_member (bwrap, "doc");
            }
        }
    }

  *out_mount_path = g_steal_pointer (&doc_mount_path);
}