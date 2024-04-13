gdk_pixbuf__tiff_image_save_to_callback (GdkPixbufSaveFunc   save_func,
                                         gpointer            user_data,
                                         GdkPixbuf          *pixbuf, 
                                         gchar             **keys,
                                         gchar             **values,
                                         GError            **error)
{
        TIFF *tiff;
        gint width, height, rowstride;
        const gchar *bits_per_sample = NULL;
        long bps;
        const gchar *compression = NULL;
        guchar *pixels;
        gboolean has_alpha;
        gushort alpha_samples[1] = { EXTRASAMPLE_UNASSALPHA };
        int y;
        TiffSaveContext *context;
        gboolean retval;
        const gchar *icc_profile = NULL;
        const gchar *x_dpi = NULL;
        const gchar *y_dpi = NULL;
        guint16 codec;

        tiff_set_handlers ();

        context = create_save_context ();
        tiff = TIFFClientOpen ("libtiff-pixbuf", "w", context,  
                               tiff_save_read, tiff_save_write, 
                               tiff_save_seek, tiff_save_close, 
                               tiff_save_size, 
                               NULL, NULL);

        if (!tiff) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("Failed to save TIFF image"));
                free_save_context (context);
                return FALSE;
        }

        rowstride = gdk_pixbuf_get_rowstride (pixbuf);
        pixels = gdk_pixbuf_get_pixels (pixbuf);

        has_alpha = gdk_pixbuf_get_has_alpha (pixbuf);

        height = gdk_pixbuf_get_height (pixbuf);
        width = gdk_pixbuf_get_width (pixbuf);

        /* Guaranteed by the caller. */
        g_assert (width >= 0);
        g_assert (height >= 0);
        g_assert (rowstride >= 0);

        TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, height);

        /* libtiff supports a number of 'codecs' such as:
           1 None, 2 Huffman, 5 LZW, 7 JPEG, 8 Deflate, see tiff.h */
        if (keys && *keys && values && *values) {
            guint i = 0;

            while (keys[i]) {
                    if (g_str_equal (keys[i], "bits-per-sample"))
                            bits_per_sample = values[i];
                    else if (g_str_equal (keys[i], "compression"))
                            compression = values[i];
                    else if (g_str_equal (keys[i], "icc-profile"))
                            icc_profile = values[i];
                    else if (g_str_equal (keys[i], "x-dpi"))
                            x_dpi = values[i];
                    else if (g_str_equal (keys[i], "y-dpi"))
                            y_dpi = values[i];
                   i++;
            }
        }

        /* Use 8 bits per sample by default, if none was recorded or
           specified. */
        if (!bits_per_sample)
                bits_per_sample = "8";

        /* Use DEFLATE compression (8) by default, if none was recorded
           or specified. */
        if (!compression)
                compression = "8";

        /* libtiff supports a number of 'codecs' such as:
           1 None, 2 Huffman, 5 LZW, 7 JPEG, 8 Deflate, see tiff.h */
        codec = strtol (compression, NULL, 0);

        if (TIFFIsCODECConfigured (codec))
                TIFFSetField (tiff, TIFFTAG_COMPRESSION, codec);
        else {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("TIFF compression doesn't refer to a valid codec."));
                retval = FALSE;
                goto cleanup;
        }

        /* We support 1-bit or 8-bit saving */
        bps = atol (bits_per_sample);
        if (bps == 1) {
                TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, 1);
                TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
                TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
        } else if (bps == 8) {
                TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, 8);
                TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, has_alpha ? 4 : 3);
                TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

                if (has_alpha)
                        TIFFSetField (tiff, TIFFTAG_EXTRASAMPLES, 1, alpha_samples);

                if (icc_profile != NULL) {
                        guchar *icc_profile_buf;
                        gsize icc_profile_size;

                        /* decode from base64 */
                        icc_profile_buf = g_base64_decode (icc_profile, &icc_profile_size);
                        if (icc_profile_size < 127) {
                                g_set_error (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_BAD_OPTION,
                                             _("Color profile has invalid length %d."),
                                             (gint) icc_profile_size);
                                retval = FALSE;
                                g_free (icc_profile_buf);
                                goto cleanup;
                        }

                        TIFFSetField (tiff, TIFFTAG_ICCPROFILE, icc_profile_size, icc_profile_buf);
                        g_free (icc_profile_buf);
                }
        } else {
                /* The passed bits-per-sample is not supported. */
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("TIFF bits-per-sample doesn't contain a supported value."));
                retval = FALSE;
                goto cleanup;
         }

        TIFFSetField (tiff, TIFFTAG_ROWSPERSTRIP, height);
        TIFFSetField (tiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
        TIFFSetField (tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

        if (bps == 1) {
                guchar *mono_row;
                gint *dith_row_1, *dith_row_2, *dith_row_tmp;

                dith_row_1 = g_new (gint, width);
                dith_row_2 = g_new (gint, width);
                mono_row = g_malloc ((width + 7) / 8);

                copy_gray_row (dith_row_1, pixels, width, has_alpha);

                for (y = 0; y < height; y++) {
                        guint x;
                        gint *p;

                        memset (mono_row, 0, (width + 7) / 8);

                        if (y > 0) {
                                dith_row_tmp = dith_row_1;
                                dith_row_1 = dith_row_2;
                                dith_row_2 = dith_row_tmp;
                        }

                        if (y < (height - 1))
                                copy_gray_row (dith_row_2, pixels + ((y + 1) * rowstride), width, has_alpha);

                        p = dith_row_1;
                        for (x = 0; x < width; x++) {
                                gint p_old, p_new, quant_error;

                                /* Apply Floyd-Steinberg dithering */

                                p_old = *p++;

                                if (p_old > 127)
                                        p_new = 255;
                                else
                                        p_new = 0;

                                quant_error = p_old - p_new;
                                if (x < (width - 1))
                                        dith_row_1[x + 1] += 7 * quant_error / 16;
                                if (y < (height - 1)) {
                                        if (x > 0)
                                                dith_row_2[x - 1] += 3 * quant_error / 16;

                                        dith_row_2[x] += 5 * quant_error / 16;

                                        if (x < (width - 1))
                                                dith_row_2[x + 1] += quant_error / 16;
                                }

                                if (p_new > 127)
                                        mono_row[x / 8] |= (0x1 << (7 - (x % 8)));
                        }

                        if (TIFFWriteScanline (tiff, mono_row, y, 0) == -1)
                                break;
                }
                g_free (mono_row);
                g_free (dith_row_1);
                g_free (dith_row_2);
        } else {
                for (y = 0; y < height; y++) {
                        if (TIFFWriteScanline (tiff, pixels + y * rowstride, y, 0) == -1)
                                break;
                }
        }

        if (y < height) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("Failed to write TIFF data"));
                TIFFClose (tiff);
                retval = FALSE;
                goto cleanup;
        }

        if (x_dpi != NULL && y_dpi != NULL) {
                char *endptr = NULL;
                uint16 resolution_unit = RESUNIT_INCH;
                float x_dpi_value, y_dpi_value;

                x_dpi_value = strtol (x_dpi, &endptr, 10);
                if (x_dpi[0] != '\0' && *endptr != '\0')
                        x_dpi_value = -1;
                if (x_dpi_value <= 0) {
                    g_set_error (error,
                                 GDK_PIXBUF_ERROR,
                                 GDK_PIXBUF_ERROR_BAD_OPTION,
                                 _("TIFF x-dpi must be greater than zero; value '%s' is not allowed."),
                                 x_dpi);
                    retval = FALSE;
                    goto cleanup;
                }
                y_dpi_value = strtol (y_dpi, &endptr, 10);
                if (y_dpi[0] != '\0' && *endptr != '\0')
                        y_dpi_value = -1;
                if (y_dpi_value <= 0) {
                    g_set_error (error,
                                 GDK_PIXBUF_ERROR,
                                 GDK_PIXBUF_ERROR_BAD_OPTION,
                                 _("TIFF y-dpi must be greater than zero; value '%s' is not allowed."),
                                 y_dpi);
                    retval = FALSE;
                    goto cleanup;
                }

                TIFFSetField (tiff, TIFFTAG_RESOLUTIONUNIT, resolution_unit);
                TIFFSetField (tiff, TIFFTAG_XRESOLUTION, x_dpi_value);
                TIFFSetField (tiff, TIFFTAG_YRESOLUTION, y_dpi_value);
        }

        TIFFClose (tiff);

        /* Now call the callback */
        retval = save_func (context->buffer, context->used, error, user_data);

cleanup:
        free_save_context (context);
        return retval;
}