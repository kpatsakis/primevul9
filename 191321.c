gst_qtdemux_configure_stream (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
  if (stream->subtype == FOURCC_vide) {
    /* fps is calculated base on the duration of the average framerate since
     * qt does not have a fixed framerate. */
    gboolean fps_available = TRUE;

    if ((stream->n_samples == 1) && (stream->first_duration == 0)) {
      /* still frame */
      stream->fps_n = 0;
      stream->fps_d = 1;
    } else {
      if (stream->duration == 0 || stream->n_samples < 2) {
        stream->fps_n = stream->timescale;
        stream->fps_d = 1;
        fps_available = FALSE;
      } else {
        GstClockTime avg_duration;
        guint64 duration;
        guint32 n_samples;

        /* duration and n_samples can be updated for fragmented format
         * so, framerate of fragmented format is calculated using data in a moof */
        if (qtdemux->fragmented && stream->n_samples_moof > 0
            && stream->duration_moof > 0) {
          n_samples = stream->n_samples_moof;
          duration = stream->duration_moof;
        } else {
          n_samples = stream->n_samples;
          duration = stream->duration;
        }

        /* Calculate a framerate, ignoring the first sample which is sometimes truncated */
        /* stream->duration is guint64, timescale, n_samples are guint32 */
        avg_duration =
            gst_util_uint64_scale_round (duration -
            stream->first_duration, GST_SECOND,
            (guint64) (stream->timescale) * (n_samples - 1));

        GST_LOG_OBJECT (qtdemux,
            "Calculating avg sample duration based on stream (or moof) duration %"
            G_GUINT64_FORMAT
            " minus first sample %u, leaving %d samples gives %"
            GST_TIME_FORMAT, duration, stream->first_duration,
            n_samples - 1, GST_TIME_ARGS (avg_duration));

        gst_video_guess_framerate (avg_duration, &stream->fps_n,
            &stream->fps_d);

        GST_DEBUG_OBJECT (qtdemux,
            "Calculating framerate, timescale %u gave fps_n %d fps_d %d",
            stream->timescale, stream->fps_n, stream->fps_d);
      }
    }

    if (stream->caps) {
      stream->caps = gst_caps_make_writable (stream->caps);

      gst_caps_set_simple (stream->caps,
          "width", G_TYPE_INT, stream->width,
          "height", G_TYPE_INT, stream->height, NULL);

      /* set framerate if calculated framerate is reliable */
      if (fps_available) {
        gst_caps_set_simple (stream->caps,
            "framerate", GST_TYPE_FRACTION, stream->fps_n, stream->fps_d, NULL);
      }

      /* calculate pixel-aspect-ratio using display width and height */
      GST_DEBUG_OBJECT (qtdemux,
          "video size %dx%d, target display size %dx%d", stream->width,
          stream->height, stream->display_width, stream->display_height);
      /* qt file might have pasp atom */
      if (stream->par_w > 0 && stream->par_h > 0) {
        GST_DEBUG_OBJECT (qtdemux, "par %d:%d", stream->par_w, stream->par_h);
        gst_caps_set_simple (stream->caps, "pixel-aspect-ratio",
            GST_TYPE_FRACTION, stream->par_w, stream->par_h, NULL);
      } else if (stream->display_width > 0 && stream->display_height > 0 &&
          stream->width > 0 && stream->height > 0) {
        gint n, d;

        /* calculate the pixel aspect ratio using the display and pixel w/h */
        n = stream->display_width * stream->height;
        d = stream->display_height * stream->width;
        if (n == d)
          n = d = 1;
        GST_DEBUG_OBJECT (qtdemux, "setting PAR to %d/%d", n, d);
        stream->par_w = n;
        stream->par_h = d;
        gst_caps_set_simple (stream->caps, "pixel-aspect-ratio",
            GST_TYPE_FRACTION, stream->par_w, stream->par_h, NULL);
      }

      if (stream->interlace_mode > 0) {
        if (stream->interlace_mode == 1) {
          gst_caps_set_simple (stream->caps, "interlace-mode", G_TYPE_STRING,
              "progressive", NULL);
        } else if (stream->interlace_mode == 2) {
          gst_caps_set_simple (stream->caps, "interlace-mode", G_TYPE_STRING,
              "interleaved", NULL);
          if (stream->field_order == 9) {
            gst_caps_set_simple (stream->caps, "field-order", G_TYPE_STRING,
                "top-field-first", NULL);
          } else if (stream->field_order == 14) {
            gst_caps_set_simple (stream->caps, "field-order", G_TYPE_STRING,
                "bottom-field-first", NULL);
          }
        }
      }

      /* Create incomplete colorimetry here if needed */
      if (stream->colorimetry.range ||
          stream->colorimetry.matrix ||
          stream->colorimetry.transfer || stream->colorimetry.primaries) {
        gchar *colorimetry =
            gst_video_colorimetry_to_string (&stream->colorimetry);
        gst_caps_set_simple (stream->caps, "colorimetry", G_TYPE_STRING,
            colorimetry, NULL);
        g_free (colorimetry);
      }

      if (stream->multiview_mode != GST_VIDEO_MULTIVIEW_MODE_NONE) {
        guint par_w = 1, par_h = 1;

        if (stream->par_w > 0 && stream->par_h > 0) {
          par_w = stream->par_w;
          par_h = stream->par_h;
        }

        if (gst_video_multiview_guess_half_aspect (stream->multiview_mode,
                stream->width, stream->height, par_w, par_h)) {
          stream->multiview_flags |= GST_VIDEO_MULTIVIEW_FLAGS_HALF_ASPECT;
        }

        gst_caps_set_simple (stream->caps,
            "multiview-mode", G_TYPE_STRING,
            gst_video_multiview_mode_to_caps_string (stream->multiview_mode),
            "multiview-flags", GST_TYPE_VIDEO_MULTIVIEW_FLAGSET,
            stream->multiview_flags, GST_FLAG_SET_MASK_EXACT, NULL);
      }
    }
  }

  else if (stream->subtype == FOURCC_soun) {
    if (stream->caps) {
      stream->caps = gst_caps_make_writable (stream->caps);
      if (stream->rate > 0)
        gst_caps_set_simple (stream->caps,
            "rate", G_TYPE_INT, (int) stream->rate, NULL);
      if (stream->n_channels > 0)
        gst_caps_set_simple (stream->caps,
            "channels", G_TYPE_INT, stream->n_channels, NULL);
      if (stream->n_channels > 2) {
        /* FIXME: Need to parse the 'chan' atom to get channel layouts
         * correctly; this is just the minimum we can do - assume
         * we don't actually have any channel positions. */
        gst_caps_set_simple (stream->caps,
            "channel-mask", GST_TYPE_BITMASK, G_GUINT64_CONSTANT (0), NULL);
      }
    }
  }

  if (stream->pad) {
    GstCaps *prev_caps = NULL;

    GST_PAD_ELEMENT_PRIVATE (stream->pad) = stream;
    gst_pad_set_event_function (stream->pad, gst_qtdemux_handle_src_event);
    gst_pad_set_query_function (stream->pad, gst_qtdemux_handle_src_query);
    gst_pad_set_active (stream->pad, TRUE);

    gst_pad_use_fixed_caps (stream->pad);

    if (stream->protected) {
      if (!gst_qtdemux_configure_protected_caps (qtdemux, stream)) {
        GST_ERROR_OBJECT (qtdemux,
            "Failed to configure protected stream caps.");
        return FALSE;
      }
    }

    if (stream->new_stream) {
      gchar *stream_id;
      GstEvent *event;
      GstStreamFlags stream_flags;

      event =
          gst_pad_get_sticky_event (qtdemux->sinkpad, GST_EVENT_STREAM_START,
          0);
      if (event) {
        if (gst_event_parse_group_id (event, &qtdemux->group_id))
          qtdemux->have_group_id = TRUE;
        else
          qtdemux->have_group_id = FALSE;
        gst_event_unref (event);
      } else if (!qtdemux->have_group_id) {
        qtdemux->have_group_id = TRUE;
        qtdemux->group_id = gst_util_group_id_next ();
      }

      stream->new_stream = FALSE;
      stream_id =
          gst_pad_create_stream_id_printf (stream->pad,
          GST_ELEMENT_CAST (qtdemux), "%03u", stream->track_id);
      event = gst_event_new_stream_start (stream_id);
      if (qtdemux->have_group_id)
        gst_event_set_group_id (event, qtdemux->group_id);
      stream_flags = GST_STREAM_FLAG_NONE;
      if (stream->disabled)
        stream_flags |= GST_STREAM_FLAG_UNSELECT;
      if (stream->sparse)
        stream_flags |= GST_STREAM_FLAG_SPARSE;
      gst_event_set_stream_flags (event, stream_flags);
      gst_pad_push_event (stream->pad, event);
      g_free (stream_id);
    }

    prev_caps = gst_pad_get_current_caps (stream->pad);

    if (!prev_caps || !gst_caps_is_equal_fixed (prev_caps, stream->caps)) {
      GST_DEBUG_OBJECT (qtdemux, "setting caps %" GST_PTR_FORMAT, stream->caps);
      gst_pad_set_caps (stream->pad, stream->caps);
    } else {
      GST_DEBUG_OBJECT (qtdemux, "ignore duplicated caps");
    }

    if (prev_caps)
      gst_caps_unref (prev_caps);
    stream->new_caps = FALSE;
  }
  return TRUE;
}