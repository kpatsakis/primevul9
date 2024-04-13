xcf_save_stream (Gimp           *gimp,
                 GimpImage      *image,
                 GOutputStream  *output,
                 GFile          *output_file,
                 GimpProgress   *progress,
                 GError        **error)
{
  XcfInfo      info     = { 0, };
  const gchar *filename;
  gboolean     success  = FALSE;
  GError      *my_error = NULL;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), FALSE);
  g_return_val_if_fail (GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (G_IS_OUTPUT_STREAM (output), FALSE);
  g_return_val_if_fail (output_file == NULL || G_IS_FILE (output_file), FALSE);
  g_return_val_if_fail (progress == NULL || GIMP_IS_PROGRESS (progress), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (output_file)
    filename = gimp_file_get_utf8_name (output_file);
  else
    filename = _("Memory Stream");

  info.gimp             = gimp;
  info.output           = output;
  info.seekable         = G_SEEKABLE (output);
  info.bytes_per_offset = 4;
  info.progress         = progress;
  info.file             = output_file;

  if (gimp_image_get_xcf_compression (image))
    info.compression = COMPRESS_ZLIB;
  else
    info.compression = COMPRESS_RLE;

  info.file_version = gimp_image_get_xcf_version (image,
                                                  info.compression ==
                                                  COMPRESS_ZLIB,
                                                  NULL, NULL);

  if (info.file_version >= 11)
    info.bytes_per_offset = 8;

  if (progress)
    gimp_progress_start (progress, FALSE, _("Saving '%s'"), filename);

  success = xcf_save_image (&info, image, &my_error);

  if (success)
    {
      if (progress)
        gimp_progress_set_text (progress, _("Closing '%s'"), filename);

      success = g_output_stream_close (info.output, NULL, &my_error);
    }

  if (! success && my_error)
    g_propagate_prefixed_error (error, my_error,
                                _("Error writing '%s': "), filename);

  if (progress)
    gimp_progress_end (progress);

  return success;
}