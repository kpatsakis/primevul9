build_attribute_list_for_copy (GFile                  *file,
                               GFileCopyFlags          flags,
                               char                  **out_attributes,
                               GCancellable           *cancellable,
                               GError                **error)
{
  gboolean ret = FALSE;
  GFileAttributeInfoList *attributes = NULL, *namespaces = NULL;
  GString *s = NULL;
  gboolean first;
  int i;
  gboolean copy_all_attributes;
  gboolean skip_perms;

  copy_all_attributes = flags & G_FILE_COPY_ALL_METADATA;
  skip_perms = (flags & G_FILE_COPY_TARGET_DEFAULT_PERMS) != 0;

  /* Ignore errors here, if the target supports no attributes there is
   * nothing to copy.  We still honor the cancellable though.
   */
  attributes = g_file_query_settable_attributes (file, cancellable, NULL);
  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    goto out;

  namespaces = g_file_query_writable_namespaces (file, cancellable, NULL);
  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    goto out;

  if (attributes == NULL && namespaces == NULL)
    goto out;

  first = TRUE;
  s = g_string_new ("");

  if (attributes)
    {
      for (i = 0; i < attributes->n_infos; i++)
        {
          if (should_copy (&attributes->infos[i], copy_all_attributes, skip_perms))
            {
              if (first)
                first = FALSE;
              else
                g_string_append_c (s, ',');

              g_string_append (s, attributes->infos[i].name);
            }
        }
    }

  if (namespaces)
    {
      for (i = 0; i < namespaces->n_infos; i++)
        {
          if (should_copy (&namespaces->infos[i], copy_all_attributes, FALSE))
            {
              if (first)
                first = FALSE;
              else
                g_string_append_c (s, ',');

              g_string_append (s, namespaces->infos[i].name);
              g_string_append (s, "::*");
            }
        }
    }

  ret = TRUE;
  *out_attributes = g_string_free (s, FALSE);
  s = NULL;
 out:
  if (s)
    g_string_free (s, TRUE);
  if (attributes)
    g_file_attribute_info_list_unref (attributes);
  if (namespaces)
    g_file_attribute_info_list_unref (namespaces);
  
  return ret;
}