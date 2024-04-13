xcf_save_invoker (GimpProcedure         *procedure,
                  Gimp                  *gimp,
                  GimpContext           *context,
                  GimpProgress          *progress,
                  const GimpValueArray  *args,
                  GError               **error)
{
  GimpValueArray *return_vals;
  GimpImage      *image;
  const gchar    *uri;
  GFile          *file;
  GOutputStream  *output;
  gboolean        success  = FALSE;
  GError         *my_error = NULL;

  gimp_set_busy (gimp);

  image = gimp_value_get_image (gimp_value_array_index (args, 1), gimp);
  uri   = g_value_get_string (gimp_value_array_index (args, 3));
  file  = g_file_new_for_uri (uri);

  output = G_OUTPUT_STREAM (g_file_replace (file,
                                            NULL, FALSE, G_FILE_CREATE_NONE,
                                            NULL, &my_error));

  if (output)
    {
      success = xcf_save_stream (gimp, image, output, file, progress, error);

      g_object_unref (output);
    }
  else
    {
      g_propagate_prefixed_error (error, my_error,
                                  _("Error creating '%s': "),
                                  gimp_file_get_utf8_name (file));
    }

  g_object_unref (file);

  return_vals = gimp_procedure_get_return_values (procedure, success,
                                                  error ? *error : NULL);

  gimp_unset_busy (gimp);

  return return_vals;
}