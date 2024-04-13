add_color_map (const gint32  image_id,
               PSDimage     *img_a)
{
  GimpParasite *parasite;

  if (img_a->color_map_len)
    {
      if (img_a->color_mode != PSD_DUOTONE)
        gimp_image_set_colormap (image_id, img_a->color_map, img_a->color_map_entries);
      else
        {
           /* Add parasite for Duotone color data */
          IFDBG(2) g_debug ("Add Duotone color data parasite");
          parasite = gimp_parasite_new (PSD_PARASITE_DUOTONE_DATA, 0,
                                        img_a->color_map_len, img_a->color_map);
          gimp_image_parasite_attach (image_id, parasite);
          gimp_parasite_free (parasite);
        }
      g_free (img_a->color_map);
    }

  return 0;
}