ppm_load_read_image(FILE       *fp,
                    pnm_struct *img)
{
    guint i;

    if (img->type == PIXMAP_RAW || img->type == PIXMAP_RAW_GRAY)
      {
        if (fread (img->data, img->bpc, img->numsamples, fp) == 0)
          return;

        /* Fix endianness if necessary */
        if (img->bpc > 1)
          {
            gushort *ptr = (gushort *) img->data;

            for (i=0; i < img->numsamples; i++)
              {
                *ptr = GUINT16_FROM_BE (*ptr);
                ptr++;
              }
          }
      }
    else
      {
        /* Plain PPM or PGM format */

        if (img->bpc == sizeof (guchar))
          {
            guchar *ptr = img->data;

            for (i = 0; i < img->numsamples; i++)
              {
                guint sample;
                if (!fscanf (fp, " %u", &sample))
                  sample = 0;
                *ptr++ = sample;
              }
          }
        else if (img->bpc == sizeof (gushort))
          {
            gushort *ptr = (gushort *) img->data;

            for (i = 0; i < img->numsamples; i++)
              {
                guint sample;
                if (!fscanf (fp, " %u", &sample))
                  sample = 0;
                *ptr++ = sample;
              }
          }
        else
          {
            g_warning ("%s: Programmer stupidity error", G_STRLOC);
          }
      }
}