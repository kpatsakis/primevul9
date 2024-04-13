g_file_replace_contents_async  (GFile               *file,
                                const char          *contents,
                                gsize                length,
                                const char          *etag,
                                gboolean             make_backup,
                                GFileCreateFlags     flags,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  GBytes *bytes;

  bytes = g_bytes_new_static (contents, length);
  g_file_replace_contents_bytes_async (file, bytes, etag, make_backup, flags,
      cancellable, callback, user_data);
  g_bytes_unref (bytes);
}