gst_matroska_demux_video_caps (GstMatroskaTrackVideoContext *
    videocontext, const gchar * codec_id, guint8 * data, guint size,
    gchar ** codec_name, guint32 * riff_fourcc)
{
  GstMatroskaTrackContext *context = (GstMatroskaTrackContext *) videocontext;
  GstCaps *caps = NULL;

  g_assert (videocontext != NULL);
  g_assert (codec_name != NULL);

  if (riff_fourcc)
    *riff_fourcc = 0;

  /* TODO: check if we have all codec types from matroska-ids.h
   *       check if we have to do more special things with codec_private
   *
   * Add support for
   *  GST_MATROSKA_CODEC_ID_VIDEO_QUICKTIME
   *  GST_MATROSKA_CODEC_ID_VIDEO_SNOW
   */

  if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_VFW_FOURCC)) {
    gst_riff_strf_vids *vids = NULL;

    if (data) {
      GstBuffer *buf = NULL;

      vids = (gst_riff_strf_vids *) data;

      /* assure size is big enough */
      if (size < 24) {
        GST_WARNING ("Too small BITMAPINFOHEADER (%d bytes)", size);
        return NULL;
      }
      if (size < sizeof (gst_riff_strf_vids)) {
        vids = g_new (gst_riff_strf_vids, 1);
        memcpy (vids, data, size);
      }

      context->dts_only = TRUE; /* VFW files only store DTS */

      /* little-endian -> byte-order */
      vids->size = GUINT32_FROM_LE (vids->size);
      vids->width = GUINT32_FROM_LE (vids->width);
      vids->height = GUINT32_FROM_LE (vids->height);
      vids->planes = GUINT16_FROM_LE (vids->planes);
      vids->bit_cnt = GUINT16_FROM_LE (vids->bit_cnt);
      vids->compression = GUINT32_FROM_LE (vids->compression);
      vids->image_size = GUINT32_FROM_LE (vids->image_size);
      vids->xpels_meter = GUINT32_FROM_LE (vids->xpels_meter);
      vids->ypels_meter = GUINT32_FROM_LE (vids->ypels_meter);
      vids->num_colors = GUINT32_FROM_LE (vids->num_colors);
      vids->imp_colors = GUINT32_FROM_LE (vids->imp_colors);

      if (size > sizeof (gst_riff_strf_vids)) { /* some extra_data */
        gsize offset = sizeof (gst_riff_strf_vids);

        buf =
            gst_buffer_new_wrapped (g_memdup ((guint8 *) vids + offset,
                size - offset), size - offset);
      }

      if (riff_fourcc)
        *riff_fourcc = vids->compression;

      caps = gst_riff_create_video_caps (vids->compression, NULL, vids,
          buf, NULL, codec_name);

      if (caps == NULL) {
        GST_WARNING ("Unhandled RIFF fourcc %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (vids->compression));
      } else {
        static GstStaticCaps intra_caps = GST_STATIC_CAPS ("image/jpeg; "
            "video/x-raw; image/png; video/x-dv; video/x-huffyuv; video/x-ffv; "
            "video/x-compressed-yuv");
        context->intra_only =
            gst_caps_can_intersect (gst_static_caps_get (&intra_caps), caps);
      }

      if (buf)
        gst_buffer_unref (buf);

      if (vids != (gst_riff_strf_vids *) data)
        g_free (vids);
    }
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_UNCOMPRESSED)) {
    GstVideoInfo info;
    GstVideoFormat format;

    gst_video_info_init (&info);
    switch (videocontext->fourcc) {
      case GST_MAKE_FOURCC ('I', '4', '2', '0'):
        format = GST_VIDEO_FORMAT_I420;
        break;
      case GST_MAKE_FOURCC ('Y', 'U', 'Y', '2'):
        format = GST_VIDEO_FORMAT_YUY2;
        break;
      case GST_MAKE_FOURCC ('Y', 'V', '1', '2'):
        format = GST_VIDEO_FORMAT_YV12;
        break;
      case GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'):
        format = GST_VIDEO_FORMAT_UYVY;
        break;
      case GST_MAKE_FOURCC ('A', 'Y', 'U', 'V'):
        format = GST_VIDEO_FORMAT_AYUV;
        break;
      case GST_MAKE_FOURCC ('Y', '8', '0', '0'):
      case GST_MAKE_FOURCC ('Y', '8', ' ', ' '):
        format = GST_VIDEO_FORMAT_GRAY8;
        break;
      case GST_MAKE_FOURCC ('R', 'G', 'B', 24):
        format = GST_VIDEO_FORMAT_RGB;
        break;
      case GST_MAKE_FOURCC ('B', 'G', 'R', 24):
        format = GST_VIDEO_FORMAT_BGR;
        break;
      default:
        GST_DEBUG ("Unknown fourcc %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (videocontext->fourcc));
        return NULL;
    }

    context->intra_only = TRUE;

    gst_video_info_set_format (&info, format, videocontext->pixel_width,
        videocontext->pixel_height);
    caps = gst_video_info_to_caps (&info);
    *codec_name = gst_pb_utils_get_codec_description (caps);
    context->alignment = 32;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG4_SP)) {
    caps = gst_caps_new_simple ("video/x-divx",
        "divxversion", G_TYPE_INT, 4, NULL);
    *codec_name = g_strdup ("MPEG-4 simple profile");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG4_ASP) ||
      !strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG4_AP)) {
    caps = gst_caps_new_simple ("video/mpeg",
        "mpegversion", G_TYPE_INT, 4,
        "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
    if (data) {
      GstBuffer *priv;

      priv = gst_buffer_new_wrapped (g_memdup (data, size), size);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
      gst_buffer_unref (priv);

      gst_codec_utils_mpeg4video_caps_set_level_and_profile (caps, data, size);
    }
    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG4_ASP))
      *codec_name = g_strdup ("MPEG-4 advanced simple profile");
    else
      *codec_name = g_strdup ("MPEG-4 advanced profile");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MSMPEG4V3)) {
#if 0
    caps = gst_caps_new_full (gst_structure_new ("video/x-divx",
            "divxversion", G_TYPE_INT, 3, NULL),
        gst_structure_new ("video/x-msmpeg",
            "msmpegversion", G_TYPE_INT, 43, NULL), NULL);
#endif
    caps = gst_caps_new_simple ("video/x-msmpeg",
        "msmpegversion", G_TYPE_INT, 43, NULL);
    *codec_name = g_strdup ("Microsoft MPEG-4 v.3");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG1) ||
      !strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG2)) {
    gint mpegversion;

    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG1))
      mpegversion = 1;
    else
      mpegversion = 2;

    caps = gst_caps_new_simple ("video/mpeg",
        "systemstream", G_TYPE_BOOLEAN, FALSE,
        "mpegversion", G_TYPE_INT, mpegversion, NULL);
    *codec_name = g_strdup_printf ("MPEG-%d video", mpegversion);
    context->postprocess_frame = gst_matroska_demux_add_mpeg_seq_header;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MJPEG)) {
    caps = gst_caps_new_empty_simple ("image/jpeg");
    *codec_name = g_strdup ("Motion-JPEG");
    context->intra_only = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEG4_AVC)) {
    caps = gst_caps_new_empty_simple ("video/x-h264");
    if (data) {
      GstBuffer *priv;

      /* First byte is the version, second is the profile indication, and third
       * is the 5 contraint_set_flags and 3 reserved bits. Fourth byte is the
       * level indication. */
      gst_codec_utils_h264_caps_set_level_and_profile (caps, data + 1,
          size - 1);

      priv = gst_buffer_new_wrapped (g_memdup (data, size), size);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
      gst_buffer_unref (priv);

      gst_caps_set_simple (caps, "stream-format", G_TYPE_STRING, "avc",
          "alignment", G_TYPE_STRING, "au", NULL);
    } else {
      GST_WARNING ("No codec data found, assuming output is byte-stream");
      gst_caps_set_simple (caps, "stream-format", G_TYPE_STRING, "byte-stream",
          NULL);
    }
    *codec_name = g_strdup ("H264");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_MPEGH_HEVC)) {
    caps = gst_caps_new_empty_simple ("video/x-h265");
    if (data) {
      GstBuffer *priv;

      gst_codec_utils_h265_caps_set_level_tier_and_profile (caps, data + 1,
          size - 1);

      priv = gst_buffer_new_wrapped (g_memdup (data, size), size);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
      gst_buffer_unref (priv);

      gst_caps_set_simple (caps, "stream-format", G_TYPE_STRING, "hvc1",
          "alignment", G_TYPE_STRING, "au", NULL);
    } else {
      GST_WARNING ("No codec data found, assuming output is byte-stream");
      gst_caps_set_simple (caps, "stream-format", G_TYPE_STRING, "byte-stream",
          NULL);
    }
    *codec_name = g_strdup ("HEVC");
  } else if ((!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO1)) ||
      (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO2)) ||
      (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO3)) ||
      (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO4))) {
    gint rmversion = -1;

    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO1))
      rmversion = 1;
    else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO2))
      rmversion = 2;
    else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO3))
      rmversion = 3;
    else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_REALVIDEO4))
      rmversion = 4;

    caps = gst_caps_new_simple ("video/x-pn-realvideo",
        "rmversion", G_TYPE_INT, rmversion, NULL);
    GST_DEBUG ("data:%p, size:0x%x", data, size);
    /* We need to extract the extradata ! */
    if (data && (size >= 0x22)) {
      GstBuffer *priv;
      guint rformat;
      guint subformat;

      subformat = GST_READ_UINT32_BE (data + 0x1a);
      rformat = GST_READ_UINT32_BE (data + 0x1e);

      priv =
          gst_buffer_new_wrapped (g_memdup (data + 0x1a, size - 0x1a),
          size - 0x1a);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, "format",
          G_TYPE_INT, rformat, "subformat", G_TYPE_INT, subformat, NULL);
      gst_buffer_unref (priv);

    }
    *codec_name = g_strdup_printf ("RealVideo %d.0", rmversion);
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_THEORA)) {
    caps = gst_caps_new_empty_simple ("video/x-theora");
    context->stream_headers =
        gst_matroska_parse_xiph_stream_headers (context->codec_priv,
        context->codec_priv_size);
    /* FIXME: mark stream as broken and skip if there are no stream headers */
    context->send_stream_headers = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_DIRAC)) {
    caps = gst_caps_new_empty_simple ("video/x-dirac");
    *codec_name = g_strdup_printf ("Dirac");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_VP8)) {
    caps = gst_caps_new_empty_simple ("video/x-vp8");
    *codec_name = g_strdup_printf ("On2 VP8");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_VP9)) {
    caps = gst_caps_new_empty_simple ("video/x-vp9");
    *codec_name = g_strdup_printf ("On2 VP9");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_AV1)) {
    caps = gst_caps_new_empty_simple ("video/x-av1");
    if (data) {
      GstBuffer *priv;

      priv = gst_buffer_new_wrapped (g_memdup (data, size), size);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
      gst_buffer_unref (priv);
    } else {
      GST_WARNING ("No AV1 codec data found!");
    }
    *codec_name = g_strdup_printf ("AOM AV1");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_VIDEO_PRORES)) {
    guint32 fourcc;
    const gchar *variant, *variant_descr = "";

    /* Expect a fourcc in the codec private data */
    if (!data || size < 4) {
      GST_WARNING ("No or too small PRORESS fourcc (%d bytes)", size);
      return NULL;
    }

    fourcc = GST_STR_FOURCC (data);
    switch (fourcc) {
      case GST_MAKE_FOURCC ('a', 'p', 'c', 's'):
        variant_descr = " 4:2:2 LT";
        variant = "lt";
        break;
      case GST_MAKE_FOURCC ('a', 'p', 'c', 'h'):
        variant = "hq";
        variant_descr = " 4:2:2 HQ";
        break;
      case GST_MAKE_FOURCC ('a', 'p', '4', 'h'):
        variant = "4444";
        variant_descr = " 4:4:4:4";
        break;
      case GST_MAKE_FOURCC ('a', 'p', 'c', 'o'):
        variant = "proxy";
        variant_descr = " 4:2:2 Proxy";
        break;
      case GST_MAKE_FOURCC ('a', 'p', 'c', 'n'):
      default:
        variant = "standard";
        variant_descr = " 4:2:2 SD";
        break;
    }

    GST_LOG ("Prores video, codec fourcc %" GST_FOURCC_FORMAT,
        GST_FOURCC_ARGS (fourcc));

    caps = gst_caps_new_simple ("video/x-prores",
        "format", G_TYPE_STRING, variant, NULL);
    *codec_name = g_strdup_printf ("Apple ProRes%s", variant_descr);
    context->postprocess_frame = gst_matroska_demux_add_prores_header;
  } else {
    GST_WARNING ("Unknown codec '%s', cannot build Caps", codec_id);
    return NULL;
  }

  if (caps != NULL) {
    int i;
    GstStructure *structure;

    for (i = 0; i < gst_caps_get_size (caps); i++) {
      structure = gst_caps_get_structure (caps, i);

      /* FIXME: use the real unit here! */
      GST_DEBUG ("video size %dx%d, target display size %dx%d (any unit)",
          videocontext->pixel_width,
          videocontext->pixel_height,
          videocontext->display_width, videocontext->display_height);

      /* pixel width and height are the w and h of the video in pixels */
      if (videocontext->pixel_width > 0 && videocontext->pixel_height > 0) {
        gint w = videocontext->pixel_width;
        gint h = videocontext->pixel_height;

        gst_structure_set (structure,
            "width", G_TYPE_INT, w, "height", G_TYPE_INT, h, NULL);
      }

      if (videocontext->display_width > 0 || videocontext->display_height > 0) {
        int n, d;

        if (videocontext->display_width <= 0)
          videocontext->display_width = videocontext->pixel_width;
        if (videocontext->display_height <= 0)
          videocontext->display_height = videocontext->pixel_height;

        /* calculate the pixel aspect ratio using the display and pixel w/h */
        n = videocontext->display_width * videocontext->pixel_height;
        d = videocontext->display_height * videocontext->pixel_width;
        GST_DEBUG ("setting PAR to %d/%d", n, d);
        gst_structure_set (structure, "pixel-aspect-ratio",
            GST_TYPE_FRACTION,
            videocontext->display_width * videocontext->pixel_height,
            videocontext->display_height * videocontext->pixel_width, NULL);
      }

      if (videocontext->default_fps > 0.0) {
        gint fps_n, fps_d;

        gst_util_double_to_fraction (videocontext->default_fps, &fps_n, &fps_d);

        GST_DEBUG ("using default fps %d/%d", fps_n, fps_d);

        gst_structure_set (structure, "framerate", GST_TYPE_FRACTION, fps_n,
            fps_d, NULL);
      } else if (context->default_duration > 0) {
        int fps_n, fps_d;

        gst_video_guess_framerate (context->default_duration, &fps_n, &fps_d);

        GST_INFO ("using default duration %" G_GUINT64_FORMAT
            " framerate %d/%d", context->default_duration, fps_n, fps_d);

        gst_structure_set (structure, "framerate", GST_TYPE_FRACTION,
            fps_n, fps_d, NULL);
      } else {
        gst_structure_set (structure, "framerate", GST_TYPE_FRACTION,
            0, 1, NULL);
      }

      switch (videocontext->interlace_mode) {
        case GST_MATROSKA_INTERLACE_MODE_PROGRESSIVE:
          gst_structure_set (structure,
              "interlace-mode", G_TYPE_STRING, "progressive", NULL);
          break;
        case GST_MATROSKA_INTERLACE_MODE_INTERLACED:
          gst_structure_set (structure,
              "interlace-mode", G_TYPE_STRING, "interleaved", NULL);

          if (videocontext->field_order != GST_VIDEO_FIELD_ORDER_UNKNOWN)
            gst_structure_set (structure, "field-order", G_TYPE_STRING,
                gst_video_field_order_to_string (videocontext->field_order),
                NULL);
          break;
        default:
          break;
      }
    }
    if (videocontext->multiview_mode != GST_VIDEO_MULTIVIEW_MODE_NONE) {
      if (gst_video_multiview_guess_half_aspect (videocontext->multiview_mode,
              videocontext->pixel_width, videocontext->pixel_height,
              videocontext->display_width * videocontext->pixel_height,
              videocontext->display_height * videocontext->pixel_width)) {
        videocontext->multiview_flags |= GST_VIDEO_MULTIVIEW_FLAGS_HALF_ASPECT;
      }
      gst_caps_set_simple (caps,
          "multiview-mode", G_TYPE_STRING,
          gst_video_multiview_mode_to_caps_string
          (videocontext->multiview_mode), "multiview-flags",
          GST_TYPE_VIDEO_MULTIVIEW_FLAGSET, videocontext->multiview_flags,
          GST_FLAG_SET_MASK_EXACT, NULL);
    }

    if (videocontext->colorimetry.range != GST_VIDEO_COLOR_RANGE_UNKNOWN ||
        videocontext->colorimetry.matrix != GST_VIDEO_COLOR_MATRIX_UNKNOWN ||
        videocontext->colorimetry.transfer != GST_VIDEO_TRANSFER_UNKNOWN ||
        videocontext->colorimetry.primaries !=
        GST_VIDEO_COLOR_PRIMARIES_UNKNOWN) {
      gchar *colorimetry =
          gst_video_colorimetry_to_string (&videocontext->colorimetry);
      gst_caps_set_simple (caps, "colorimetry", G_TYPE_STRING, colorimetry,
          NULL);
      GST_DEBUG ("setting colorimetry to %s", colorimetry);
      g_free (colorimetry);
    }

    if (videocontext->mastering_display_info_present) {
      if (!gst_video_mastering_display_info_add_to_caps
          (&videocontext->mastering_display_info, caps)) {
        GST_WARNING ("couldn't set mastering display info to caps");
      }
    }

    if (videocontext->content_light_level.max_content_light_level &&
        videocontext->content_light_level.max_frame_average_light_level) {
      if (!gst_video_content_light_level_add_to_caps
          (&videocontext->content_light_level, caps)) {
        GST_WARNING ("couldn't set content light level to caps");
      }
    }

    caps = gst_caps_simplify (caps);
  }

  return caps;
}