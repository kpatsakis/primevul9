save_image (const gchar  *filename,
            gint32        image_ID,
            gint32        drawable_ID,
            GError      **error)
{
  FILE          *ofp;
  GimpImageType  drawable_type;
  gint           retval;

  drawable_type = gimp_drawable_type (drawable_ID);

  /*  Make sure we're not saving an image with an alpha channel  */
  if (gimp_drawable_has_alpha (drawable_ID))
    {
      g_message (_("Cannot save images with alpha channels."));
      return FALSE;
    }

  switch (drawable_type)
    {
    case GIMP_INDEXED_IMAGE:
    case GIMP_GRAY_IMAGE:
    case GIMP_RGB_IMAGE:
      break;
    default:
      g_message (_("Cannot operate on unknown image types."));
      return FALSE;
      break;
    }

  /* Open the output file. */
  ofp = g_fopen (filename, "wb");
  if (! ofp)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for writing: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return FALSE;
    }

  gimp_progress_init_printf (_("Saving '%s'"),
                             gimp_filename_to_utf8 (filename));

  switch (drawable_type)
    {
    case GIMP_INDEXED_IMAGE:
      retval = save_index (ofp, image_ID, drawable_ID, FALSE);
      break;
    case GIMP_GRAY_IMAGE:
      retval = save_index (ofp, image_ID, drawable_ID, TRUE);
      break;
    case GIMP_RGB_IMAGE:
      retval = save_rgb (ofp, image_ID, drawable_ID);
      break;
    default:
      retval = FALSE;
    }

  gimp_progress_update (1.0);

  fclose (ofp);

  return retval;
}