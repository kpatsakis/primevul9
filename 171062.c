g_file_has_parent (GFile *file,
                   GFile *parent)
{
  GFile *actual_parent;
  gboolean result;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (parent == NULL || G_IS_FILE (parent), FALSE);

  actual_parent = g_file_get_parent (file);

  if (actual_parent != NULL)
    {
      if (parent != NULL)
        result = g_file_equal (parent, actual_parent);
      else
        result = TRUE;

      g_object_unref (actual_parent);
    }
  else
    result = FALSE;

  return result;
}