g_file_real_set_attributes_from_info (GFile                *file,
                                      GFileInfo            *info,
                                      GFileQueryInfoFlags   flags,
                                      GCancellable         *cancellable,
                                      GError              **error)
{
  char **attributes;
  int i;
  gboolean res;
  GFileAttributeValue *value;

  res = TRUE;

  attributes = g_file_info_list_attributes (info, NULL);

  for (i = 0; attributes[i] != NULL; i++)
    {
      value = _g_file_info_get_attribute_value (info, attributes[i]);

      if (value->status != G_FILE_ATTRIBUTE_STATUS_UNSET)
        continue;

      if (!g_file_set_attribute (file, attributes[i],
                                 value->type, _g_file_attribute_value_peek_as_pointer (value),
                                 flags, cancellable, error))
        {
          value->status = G_FILE_ATTRIBUTE_STATUS_ERROR_SETTING;
          res = FALSE;
          /* Don't set error multiple times */
          error = NULL;
        }
      else
        value->status = G_FILE_ATTRIBUTE_STATUS_SET;
    }

  g_strfreev (attributes);

  return res;
}