g_file_make_directory_with_parents (GFile         *file,
                                    GCancellable  *cancellable,
                                    GError       **error)
{
  GFile *work_file = NULL;
  GList *list = NULL, *l;
  GError *my_error = NULL;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  /* Try for the simple case of not having to create any parent
   * directories.  If any parent directory needs to be created, this
   * call will fail with NOT_FOUND. If that happens, then that value of
   * my_error persists into the while loop below.
   */
  g_file_make_directory (file, cancellable, &my_error);
  if (!g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      if (my_error)
        g_propagate_error (error, my_error);
      return my_error == NULL;
    }

  work_file = g_object_ref (file);

  /* Creates the parent directories as needed. In case any particular
   * creation operation fails for lack of other parent directories
   * (NOT_FOUND), the directory is added to a list of directories to
   * create later, and the value of my_error is retained until the next
   * iteration of the loop.  After the loop my_error should either be
   * empty or contain a real failure condition.
   */
  while (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
    {
      GFile *parent_file;

      parent_file = g_file_get_parent (work_file);
      if (parent_file == NULL)
        break;

      g_clear_error (&my_error);
      g_file_make_directory (parent_file, cancellable, &my_error);
      /* Another process may have created the directory in between the
       * G_IO_ERROR_NOT_FOUND and now
       */
      if (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_EXISTS))
        g_clear_error (&my_error);

      g_object_unref (work_file);
      work_file = g_object_ref (parent_file);

      if (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        list = g_list_prepend (list, parent_file);  /* Transfer ownership of ref */
      else
        g_object_unref (parent_file);
    }

  /* All directories should be able to be created now, so an error at
   * this point means the whole operation must fail -- except an EXISTS
   * error, which means that another process already created the
   * directory in between the previous failure and now.
   */
  for (l = list; my_error == NULL && l; l = l->next)
    {
      g_file_make_directory ((GFile *) l->data, cancellable, &my_error);
      if (g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_EXISTS))
        g_clear_error (&my_error);
    }

  if (work_file)
    g_object_unref (work_file);

  /* Clean up */
  while (list != NULL)
    {
      g_object_unref ((GFile *) list->data);
      list = g_list_remove (list, list->data);
    }

  /* At this point an error in my_error means a that something
   * unexpected failed in either of the loops above, so the whole
   * operation must fail.
   */
  if (my_error != NULL)
    {
      g_propagate_error (error, my_error);
      return FALSE;
    }

  return g_file_make_directory (file, cancellable, error);
}