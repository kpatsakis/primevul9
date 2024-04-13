load_image (const gchar  *filename,
            GError      **load_error)
{
  FILE                 *f;
  struct stat           st;
  PSDimage              img_a;
  PSDlayer            **lyr_a;
  gint32                image_id = -1;
  GError               *error = NULL;

  /* ----- Open PSD file ----- */
  if (g_stat (filename, &st) == -1)
    return -1;

  IFDBG(1) g_debug ("Open file %s", gimp_filename_to_utf8 (filename));
  f = g_fopen (filename, "rb");
  if (f == NULL)
    {
      g_set_error (load_error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for reading: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return -1;
    }

  gimp_progress_init_printf (_("Opening '%s'"),
                             gimp_filename_to_utf8 (filename));

  /* ----- Read the PSD file Header block ----- */
  IFDBG(2) g_debug ("Read header block");
  if (read_header_block (&img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.1);

  /* ----- Read the PSD file Colour Mode block ----- */
  IFDBG(2) g_debug ("Read colour mode block");
  if (read_color_mode_block (&img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.2);

  /* ----- Read the PSD file Image Resource block ----- */
  IFDBG(2) g_debug ("Read image resource block");
  if (read_image_resource_block (&img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.3);

  /* ----- Read the PSD file Layer & Mask block ----- */
  IFDBG(2) g_debug ("Read layer & mask block");
  lyr_a = read_layer_block (&img_a, f, &error);
  if (img_a.num_layers != 0 && lyr_a == NULL)
    goto load_error;
  gimp_progress_update (0.4);

  /* ----- Read the PSD file Merged Image Data block ----- */
  IFDBG(2) g_debug ("Read merged image and extra alpha channel block");
  if (read_merged_image_block (&img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.5);

  /* ----- Create GIMP image ----- */
  IFDBG(2) g_debug ("Create GIMP image");
  image_id = create_gimp_image (&img_a, filename);
  if (image_id < 0)
    goto load_error;
  gimp_progress_update (0.6);

  /* ----- Add colour map ----- */
  IFDBG(2) g_debug ("Add color map");
  if (add_color_map (image_id, &img_a) < 0)
    goto load_error;
  gimp_progress_update (0.7);

  /* ----- Add image resources ----- */
  IFDBG(2) g_debug ("Add image resources");
  if (add_image_resources (image_id, &img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.8);

  /* ----- Add layers -----*/
  IFDBG(2) g_debug ("Add layers");
  if (add_layers (image_id, &img_a, lyr_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (0.9);

  /* ----- Add merged image data and extra alpha channels ----- */
  IFDBG(2) g_debug ("Add merged image data and extra alpha channels");
  if (add_merged_image (image_id, &img_a, f, &error) < 0)
    goto load_error;
  gimp_progress_update (1.0);

  IFDBG(2) g_debug ("Close file & return, image id: %d", image_id);
  IFDBG(1) g_debug ("\n----------------------------------------"
                    "----------------------------------------\n");

  gimp_image_clean_all (image_id);
  gimp_image_undo_enable (image_id);
  fclose (f);
  return image_id;

  /* ----- Process load errors ----- */
 load_error:
  if (error)
    {
      g_set_error (load_error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("Error loading PSD file: %s"), error->message);
      g_error_free (error);
    }

  /* Delete partially loaded image */
  if (image_id > 0)
    gimp_image_delete (image_id);

  /* Close file if Open */
  if (! (f == NULL))
    fclose (f);

  return -1;
}