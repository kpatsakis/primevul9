file_peek_path_generic (GFile *file)
{
  const char *path;
  static GQuark _file_path_quark = 0;

  if (G_UNLIKELY (_file_path_quark) == 0)
    _file_path_quark = g_quark_from_static_string ("gio-file-path");

  /* We need to be careful about threading, as two threads calling
   * g_file_peek_path() on the same file could race: both would see
   * (g_object_get_qdata(…) == NULL) to begin with, both would generate and add
   * the path, but the second thread to add it would end up freeing the path
   * set by the first thread. The first thread would still return the pointer
   * to that freed path, though, resulting an a read-after-free. Handle that
   * with a compare-and-swap loop. The g_object_*_qdata() functions are atomic. */

  while (TRUE)
    {
      gchar *new_path = NULL;

      path = g_object_get_qdata ((GObject*)file, _file_path_quark);

      if (path != NULL)
        break;

      if (g_file_has_uri_scheme (file, "trash") ||
          g_file_has_uri_scheme (file, "recent"))
        new_path = file_get_target_path (file);
      else
        new_path = g_file_get_path (file);
      if (new_path == NULL)
        return NULL;

      /* By passing NULL here, we ensure we never replace existing data: */
      if (g_object_replace_qdata ((GObject *) file, _file_path_quark,
                                  NULL, (gpointer) new_path,
                                  (GDestroyNotify) g_free, NULL))
        break;
      else
        g_free (new_path);
    }

  return path;
}