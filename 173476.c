add_image_resources (const gint32  image_id,
                     PSDimage     *img_a,
                     FILE         *f,
                     GError      **error)
{
  PSDimageres  res_a;

  if (fseek (f, img_a->image_res_start, SEEK_SET) < 0)
    {
      psd_set_error (feof (f), errno, error);
      return -1;
    }

  /* Initialise image resource variables */
  img_a->no_icc = FALSE;
  img_a->layer_state = 0;
  img_a->alpha_names = NULL;
  img_a->alpha_display_info = NULL;
  img_a->alpha_display_count = 0;
  img_a->alpha_id = NULL;
  img_a->alpha_id_count = 0;
  img_a->quick_mask_id = 0;

  while (ftell (f) < img_a->image_res_start + img_a->image_res_len)
    {
      if (get_image_resource_header (&res_a, f, error) < 0)
        return -1;

      if (res_a.data_start + res_a.data_len >
          img_a->image_res_start + img_a->image_res_len)
        {
          IFDBG(1) g_debug ("Unexpected end of image resource data");
          return 0;
        }

      if (load_image_resource (&res_a, image_id, img_a, f, error) < 0)
        return -1;
    }

  return 0;
}