forward_file (XdpDbusDocuments *documents,
              const char       *app_id,
              const char       *file,
              char            **out_doc_id,
              GError          **error)
{
  int fd, fd_id;
  g_autofree char *doc_id = NULL;
  g_autoptr(GUnixFDList) fd_list = NULL;
  const char *perms[] = { "read", "write", NULL };

  fd = open (file, O_PATH | O_CLOEXEC);
  if (fd == -1)
    return flatpak_fail (error, _("Failed to open ‘%s’"), file);

  fd_list = g_unix_fd_list_new ();
  fd_id = g_unix_fd_list_append (fd_list, fd, error);
  close (fd);

  if (!xdp_dbus_documents_call_add_sync (documents,
                                         g_variant_new ("h", fd_id),
                                         TRUE, /* reuse */
                                         FALSE, /* not persistent */
                                         fd_list,
                                         &doc_id,
                                         NULL,
                                         NULL,
                                         error))
    {
      if (error)
        g_dbus_error_strip_remote_error (*error);
      return FALSE;
    }

  if (!xdp_dbus_documents_call_grant_permissions_sync (documents,
                                                       doc_id,
                                                       app_id,
                                                       perms,
                                                       NULL,
                                                       error))
    {
      if (error)
        g_dbus_error_strip_remote_error (*error);
      return FALSE;
    }

  *out_doc_id = g_steal_pointer (&doc_id);

  return TRUE;
}