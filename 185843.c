xcf_load_invoker (GimpProcedure         *procedure,
                  Gimp                  *gimp,
                  GimpContext           *context,
                  GimpProgress          *progress,
                  const GimpValueArray  *args,
                  GError               **error)
{
  GimpValueArray *return_vals;
  GimpImage      *image = NULL;
  const gchar    *uri;
  GFile          *file;
  GInputStream   *input;
  GError         *my_error = NULL;

  gimp_set_busy (gimp);

  uri  = g_value_get_string (gimp_value_array_index (args, 1));
  file = g_file_new_for_uri (uri);

  input = G_INPUT_STREAM (g_file_read (file, NULL, &my_error));

  if (input)
    {
      image = xcf_load_stream (gimp, input, file, progress, error);

      g_object_unref (input);
    }
  else
    {
      g_propagate_prefixed_error (error, my_error,
                                  _("Could not open '%s' for reading: "),
                                  gimp_file_get_utf8_name (file));
    }

  g_object_unref (file);

  return_vals = gimp_procedure_get_return_values (procedure, image != NULL,
                                                  error ? *error : NULL);

  if (image)
    gimp_value_set_image (gimp_value_array_index (return_vals, 1), image);

  gimp_unset_busy (gimp);

  return return_vals;
}