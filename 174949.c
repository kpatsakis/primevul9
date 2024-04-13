ReadBMP (const gchar  *name,
         GError      **error)
{
  FILE     *fd;
  guchar    buffer[64];
  gint      ColormapSize, rowbytes, Maps;
  gboolean  Grey = FALSE;
  guchar    ColorMap[256][3];
  gint32    image_ID;
  gchar     magick[2];
  Bitmap_Channel masks[4];

  filename = name;
  fd = g_fopen (filename, "rb");

  if (!fd)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for reading: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return -1;
    }

  gimp_progress_init_printf (_("Opening '%s'"),
                             gimp_filename_to_utf8 (name));

  /* It is a File. Now is it a Bitmap? Read the shortest possible header */

  if (!ReadOK (fd, magick, 2) || !(!strncmp (magick, "BA", 2) ||
     !strncmp (magick, "BM", 2) || !strncmp (magick, "IC", 2) ||
     !strncmp (magick, "PI", 2) || !strncmp (magick, "CI", 2) ||
     !strncmp (magick, "CP", 2)))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  while (!strncmp (magick, "BA", 2))
    {
      if (!ReadOK (fd, buffer, 12))
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("'%s' is not a valid BMP file"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }
      if (!ReadOK (fd, magick, 2))
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("'%s' is not a valid BMP file"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }
    }

  if (!ReadOK (fd, buffer, 12))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  /* bring them to the right byteorder. Not too nice, but it should work */

  Bitmap_File_Head.bfSize    = ToL (&buffer[0x00]);
  Bitmap_File_Head.zzHotX    = ToS (&buffer[0x04]);
  Bitmap_File_Head.zzHotY    = ToS (&buffer[0x06]);
  Bitmap_File_Head.bfOffs    = ToL (&buffer[0x08]);

  if (!ReadOK (fd, buffer, 4))
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  Bitmap_File_Head.biSize    = ToL (&buffer[0x00]);

  /* What kind of bitmap is it? */

  if (Bitmap_File_Head.biSize == 12) /* OS/2 1.x ? */
    {
      if (!ReadOK (fd, buffer, 8))
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("Error reading BMP file header from '%s'"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }

      Bitmap_Head.biWidth   = ToS (&buffer[0x00]);       /* 12 */
      Bitmap_Head.biHeight  = ToS (&buffer[0x02]);       /* 14 */
      Bitmap_Head.biPlanes  = ToS (&buffer[0x04]);       /* 16 */
      Bitmap_Head.biBitCnt  = ToS (&buffer[0x06]);       /* 18 */
      Bitmap_Head.biCompr   = 0;
      Bitmap_Head.biSizeIm  = 0;
      Bitmap_Head.biXPels   = Bitmap_Head.biYPels = 0;
      Bitmap_Head.biClrUsed = 0;
      Bitmap_Head.biClrImp  = 0;
      Bitmap_Head.masks[0]  = 0;
      Bitmap_Head.masks[1]  = 0;
      Bitmap_Head.masks[2]  = 0;
      Bitmap_Head.masks[3]  = 0;

      memset(masks, 0, sizeof(masks));
      Maps = 3;
    }
  else if (Bitmap_File_Head.biSize == 40) /* Windows 3.x */
    {
      if (!ReadOK (fd, buffer, 36))
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("Error reading BMP file header from '%s'"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }

      Bitmap_Head.biWidth   = ToL (&buffer[0x00]);      /* 12 */
      Bitmap_Head.biHeight  = ToL (&buffer[0x04]);      /* 16 */
      Bitmap_Head.biPlanes  = ToS (&buffer[0x08]);       /* 1A */
      Bitmap_Head.biBitCnt  = ToS (&buffer[0x0A]);      /* 1C */
      Bitmap_Head.biCompr   = ToL (&buffer[0x0C]);      /* 1E */
      Bitmap_Head.biSizeIm  = ToL (&buffer[0x10]);      /* 22 */
      Bitmap_Head.biXPels   = ToL (&buffer[0x14]);      /* 26 */
      Bitmap_Head.biYPels   = ToL (&buffer[0x18]);      /* 2A */
      Bitmap_Head.biClrUsed = ToL (&buffer[0x1C]);      /* 2E */
      Bitmap_Head.biClrImp  = ToL (&buffer[0x20]);      /* 32 */
      Bitmap_Head.masks[0]  = 0;
      Bitmap_Head.masks[1]  = 0;
      Bitmap_Head.masks[2]  = 0;
      Bitmap_Head.masks[3]  = 0;

      Maps = 4;
      memset(masks, 0, sizeof(masks));

      if (Bitmap_Head.biCompr == BI_BITFIELDS)
        {
          if (!ReadOK (fd, buffer, 3 * sizeof (guint32)))
            {
              g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                           _("Error reading BMP file header from '%s'"),
                           gimp_filename_to_utf8 (filename));
              return -1;
            }

          Bitmap_Head.masks[0] = ToL(&buffer[0x00]);
          Bitmap_Head.masks[1] = ToL(&buffer[0x04]);
          Bitmap_Head.masks[2] = ToL(&buffer[0x08]);
         ReadChannelMasks (&Bitmap_Head.masks[0], masks, 3);
        }
      else
        switch (Bitmap_Head.biBitCnt)
          {
          case 32:
            masks[0].mask     = 0x00ff0000;
            masks[0].shiftin  = 16;
            masks[0].max_value= (gfloat)255.0;
            masks[1].mask     = 0x0000ff00;
            masks[1].shiftin  = 8;
            masks[1].max_value= (gfloat)255.0;
            masks[2].mask     = 0x000000ff;
            masks[2].shiftin  = 0;
            masks[2].max_value= (gfloat)255.0;
            masks[3].mask     = 0xff000000;
            masks[3].shiftin  = 24;
            masks[3].max_value= (gfloat)255.0;
            break;
         case 24:
            masks[0].mask     = 0xff0000;
            masks[0].shiftin  = 16;
            masks[0].max_value= (gfloat)255.0;
            masks[1].mask     = 0x00ff00;
            masks[1].shiftin  = 8;
            masks[1].max_value= (gfloat)255.0;
            masks[2].mask     = 0x0000ff;
            masks[2].shiftin  = 0;
            masks[2].max_value= (gfloat)255.0;
            masks[3].mask     = 0x0;
            masks[3].shiftin  = 0;
            masks[3].max_value= (gfloat)0.0;
            break;
         case 16:
            masks[0].mask     = 0x7c00;
            masks[0].shiftin  = 10;
            masks[0].max_value= (gfloat)31.0;
            masks[1].mask     = 0x03e0;
            masks[1].shiftin  = 5;
            masks[1].max_value= (gfloat)31.0;
            masks[2].mask     = 0x001f;
            masks[2].shiftin  = 0;
            masks[2].max_value= (gfloat)31.0;
            masks[3].mask     = 0x0;
            masks[3].shiftin  = 0;
            masks[3].max_value= (gfloat)0.0;
            break;
         default:
            break;
         }
    }
  else if (Bitmap_File_Head.biSize >= 56 && Bitmap_File_Head.biSize <= 64)
    /* enhanced Windows format with bit masks */
    {
      if (!ReadOK (fd, buffer, Bitmap_File_Head.biSize - 4))
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("Error reading BMP file header from '%s'"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }

      Bitmap_Head.biWidth   =ToL (&buffer[0x00]);       /* 12 */
      Bitmap_Head.biHeight  =ToL (&buffer[0x04]);       /* 16 */
      Bitmap_Head.biPlanes  =ToS (&buffer[0x08]);       /* 1A */
      Bitmap_Head.biBitCnt  =ToS (&buffer[0x0A]);       /* 1C */
      Bitmap_Head.biCompr   =ToL (&buffer[0x0C]);       /* 1E */
      Bitmap_Head.biSizeIm  =ToL (&buffer[0x10]);       /* 22 */
      Bitmap_Head.biXPels   =ToL (&buffer[0x14]);       /* 26 */
      Bitmap_Head.biYPels   =ToL (&buffer[0x18]);       /* 2A */
      Bitmap_Head.biClrUsed =ToL (&buffer[0x1C]);       /* 2E */
      Bitmap_Head.biClrImp  =ToL (&buffer[0x20]);       /* 32 */
      Bitmap_Head.masks[0]  =ToL (&buffer[0x24]);       /* 36 */
      Bitmap_Head.masks[1]  =ToL (&buffer[0x28]);       /* 3A */
      Bitmap_Head.masks[2]  =ToL (&buffer[0x2C]);       /* 3E */
      Bitmap_Head.masks[3]  =ToL (&buffer[0x30]);       /* 42 */

      Maps = 4;
      ReadChannelMasks (&Bitmap_Head.masks[0], masks, 4);
    }
  else
    {
      GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

      if (pixbuf)
        {
          gint32 layer_ID;

          image_ID = gimp_image_new (gdk_pixbuf_get_width (pixbuf),
                                     gdk_pixbuf_get_height (pixbuf),
                                     GIMP_RGB);

          layer_ID = gimp_layer_new_from_pixbuf (image_ID, _("Background"),
                                                 pixbuf,
                                                 100.,
                                                 GIMP_NORMAL_MODE, 0, 0);
          g_object_unref (pixbuf);

          gimp_image_set_filename (image_ID, filename);
          gimp_image_add_layer (image_ID, layer_ID, -1);

          return image_ID;
        }
      else
        {

          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                       _("Error reading BMP file header from '%s'"),
                       gimp_filename_to_utf8 (filename));
          return -1;
        }
    }

  /* Valid bitpdepthis 1, 4, 8, 16, 24, 32 */
  /* 16 is awful, we should probably shoot whoever invented it */

  /* There should be some colors used! */

  ColormapSize =
    (Bitmap_File_Head.bfOffs - Bitmap_File_Head.biSize - 14) / Maps;

  if ((Bitmap_Head.biClrUsed == 0) && (Bitmap_Head.biBitCnt <= 8))
    ColormapSize = Bitmap_Head.biClrUsed = 1 << Bitmap_Head.biBitCnt;

  if (ColormapSize > 256)
    ColormapSize = 256;

  /* Sanity checks */

  if (Bitmap_Head.biHeight == 0 || Bitmap_Head.biWidth == 0)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  if (Bitmap_Head.biWidth < 0 ||
      ABS (Bitmap_Head.biHeight) < 0)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  if (Bitmap_Head.biPlanes != 1)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  if (Bitmap_Head.biClrUsed > 256)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  /* protect against integer overflows caused by malicious BMPs */

  if (((guint64) Bitmap_Head.biWidth) * Bitmap_Head.biBitCnt > G_MAXINT32 ||
      ((guint64) Bitmap_Head.biWidth) * ABS (Bitmap_Head.biHeight) > G_MAXINT32 ||
      ((guint64) Bitmap_Head.biWidth) * ABS (Bitmap_Head.biHeight) * 4 > G_MAXINT32)
    {
      g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                   _("'%s' is not a valid BMP file"),
                   gimp_filename_to_utf8 (filename));
      return -1;
    }

  /* Windows and OS/2 declare filler so that rows are a multiple of
   * word length (32 bits == 4 bytes)
   */

  rowbytes= ((Bitmap_Head.biWidth * Bitmap_Head.biBitCnt - 1) / 32) * 4 + 4;

#ifdef DEBUG
  printf ("\nSize: %u, Colors: %u, Bits: %u, Width: %u, Height: %u, "
          "Comp: %u, Zeile: %u\n",
          Bitmap_File_Head.bfSize,
          Bitmap_Head.biClrUsed,
          Bitmap_Head.biBitCnt,
          Bitmap_Head.biWidth,
          Bitmap_Head.biHeight,
          Bitmap_Head.biCompr,
          rowbytes);
#endif

  if (Bitmap_Head.biBitCnt <= 8)
    {
#ifdef DEBUG
      printf ("Colormap read\n");
#endif
      /* Get the Colormap */
      if (!ReadColorMap (fd, ColorMap, ColormapSize, Maps, &Grey))
        return -1;
    }

  fseek (fd, Bitmap_File_Head.bfOffs, SEEK_SET);

  /* Get the Image and return the ID or -1 on error*/
  image_ID = ReadImage (fd,
                        Bitmap_Head.biWidth,
                        ABS (Bitmap_Head.biHeight),
                        ColorMap,
                        Bitmap_Head.biClrUsed,
                        Bitmap_Head.biBitCnt,
                        Bitmap_Head.biCompr,
                        rowbytes,
                        Grey,
                        masks,
                        error);

  if (image_ID < 0)
    return -1;

  if (Bitmap_Head.biXPels > 0 && Bitmap_Head.biYPels > 0)
    {
      /* Fixed up from scott@asofyet's changes last year, njl195 */
      gdouble xresolution;
      gdouble yresolution;

      /* I don't agree with scott's feeling that Gimp should be
       * trying to "fix" metric resolution translations, in the
       * long term Gimp should be SI (metric) anyway, but we
       * haven't told the Americans that yet  */

      xresolution = Bitmap_Head.biXPels * 0.0254;
      yresolution = Bitmap_Head.biYPels * 0.0254;

      gimp_image_set_resolution (image_ID, xresolution, yresolution);
    }

  if (Bitmap_Head.biHeight < 0)
    gimp_image_flip (image_ID, GIMP_ORIENTATION_VERTICAL);

  return image_ID;
}