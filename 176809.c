gst_matroska_demux_audio_caps (GstMatroskaTrackAudioContext *
    audiocontext, const gchar * codec_id, guint8 * data, guint size,
    gchar ** codec_name, guint16 * riff_audio_fmt, GstClockTime * lead_in_ts)
{
  GstMatroskaTrackContext *context = (GstMatroskaTrackContext *) audiocontext;
  GstCaps *caps = NULL;
  guint lead_in = 0;
  /* Max potential blocksize causing the longest possible lead_in_ts need, as
   * we don't have the exact number parsed out here */
  guint max_blocksize = 0;
  /* Original samplerate before SBR multiplications, as parsers would use */
  guint rate = audiocontext->samplerate;

  g_assert (audiocontext != NULL);
  g_assert (codec_name != NULL);

  if (riff_audio_fmt)
    *riff_audio_fmt = 0;

  /* TODO: check if we have all codec types from matroska-ids.h
   *       check if we have to do more special things with codec_private
   *       check if we need bitdepth in different places too
   *       implement channel position magic
   * Add support for:
   *  GST_MATROSKA_CODEC_ID_AUDIO_AC3_BSID9
   *  GST_MATROSKA_CODEC_ID_AUDIO_AC3_BSID10
   *  GST_MATROSKA_CODEC_ID_AUDIO_QUICKTIME_QDMC
   *  GST_MATROSKA_CODEC_ID_AUDIO_QUICKTIME_QDM2
   */

  if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_MPEG1_L1) ||
      !strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_MPEG1_L2) ||
      !strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_MPEG1_L3)) {
    gint layer;

    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_MPEG1_L1))
      layer = 1;
    else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_MPEG1_L2))
      layer = 2;
    else
      layer = 3;

    lead_in = 30;               /* Could mp2 need as much too? */
    max_blocksize = 1152;
    caps = gst_caps_new_simple ("audio/mpeg",
        "mpegversion", G_TYPE_INT, 1, "layer", G_TYPE_INT, layer, NULL);
    *codec_name = g_strdup_printf ("MPEG-1 layer %d", layer);
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_PCM_INT_BE) ||
      !strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_PCM_INT_LE)) {
    gboolean sign;
    gint endianness;
    GstAudioFormat format;

    sign = (audiocontext->bitdepth != 8);
    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_PCM_INT_BE))
      endianness = G_BIG_ENDIAN;
    else
      endianness = G_LITTLE_ENDIAN;

    format = gst_audio_format_build_integer (sign, endianness,
        audiocontext->bitdepth, audiocontext->bitdepth);

    /* FIXME: Channel mask and reordering */
    caps = gst_caps_new_simple ("audio/x-raw",
        "format", G_TYPE_STRING, gst_audio_format_to_string (format),
        "layout", G_TYPE_STRING, "interleaved",
        "channel-mask", GST_TYPE_BITMASK,
        gst_audio_channel_get_fallback_mask (audiocontext->channels), NULL);

    *codec_name = g_strdup_printf ("Raw %d-bit PCM audio",
        audiocontext->bitdepth);
    context->alignment = GST_ROUND_UP_8 (audiocontext->bitdepth) / 8;
    context->alignment = round_up_pow2 (context->alignment);
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_PCM_FLOAT)) {
    const gchar *format;
    if (audiocontext->bitdepth == 32)
      format = "F32LE";
    else
      format = "F64LE";
    /* FIXME: Channel mask and reordering */
    caps = gst_caps_new_simple ("audio/x-raw",
        "format", G_TYPE_STRING, format,
        "layout", G_TYPE_STRING, "interleaved",
        "channel-mask", GST_TYPE_BITMASK,
        gst_audio_channel_get_fallback_mask (audiocontext->channels), NULL);
    *codec_name = g_strdup_printf ("Raw %d-bit floating-point audio",
        audiocontext->bitdepth);
    context->alignment = audiocontext->bitdepth / 8;
    context->alignment = round_up_pow2 (context->alignment);
  } else if (!strncmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_AC3,
          strlen (GST_MATROSKA_CODEC_ID_AUDIO_AC3))) {
    lead_in = 2;
    max_blocksize = 1536;
    caps = gst_caps_new_simple ("audio/x-ac3",
        "framed", G_TYPE_BOOLEAN, TRUE, NULL);
    *codec_name = g_strdup ("AC-3 audio");
  } else if (!strncmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_EAC3,
          strlen (GST_MATROSKA_CODEC_ID_AUDIO_EAC3))) {
    lead_in = 2;
    max_blocksize = 1536;
    caps = gst_caps_new_simple ("audio/x-eac3",
        "framed", G_TYPE_BOOLEAN, TRUE, NULL);
    *codec_name = g_strdup ("E-AC-3 audio");
  } else if (!strncmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_TRUEHD,
          strlen (GST_MATROSKA_CODEC_ID_AUDIO_TRUEHD))) {
    caps = gst_caps_new_empty_simple ("audio/x-true-hd");
    *codec_name = g_strdup ("Dolby TrueHD");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_DTS)) {
    caps = gst_caps_new_empty_simple ("audio/x-dts");
    *codec_name = g_strdup ("DTS audio");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_VORBIS)) {
    caps = gst_caps_new_empty_simple ("audio/x-vorbis");
    context->stream_headers =
        gst_matroska_parse_xiph_stream_headers (context->codec_priv,
        context->codec_priv_size);
    /* FIXME: mark stream as broken and skip if there are no stream headers */
    context->send_stream_headers = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_FLAC)) {
    caps = gst_caps_new_empty_simple ("audio/x-flac");
    context->stream_headers =
        gst_matroska_parse_flac_stream_headers (context->codec_priv,
        context->codec_priv_size);
    /* FIXME: mark stream as broken and skip if there are no stream headers */
    context->send_stream_headers = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_SPEEX)) {
    caps = gst_caps_new_empty_simple ("audio/x-speex");
    context->stream_headers =
        gst_matroska_parse_speex_stream_headers (context->codec_priv,
        context->codec_priv_size);
    /* FIXME: mark stream as broken and skip if there are no stream headers */
    context->send_stream_headers = TRUE;
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_OPUS)) {
    GstBuffer *tmp;

    if (context->codec_priv_size >= 19) {
      if (audiocontext->samplerate)
        GST_WRITE_UINT32_LE ((guint8 *) context->codec_priv + 12,
            audiocontext->samplerate);
      if (context->codec_delay) {
        guint64 delay =
            gst_util_uint64_scale_round (context->codec_delay, 48000,
            GST_SECOND);
        GST_WRITE_UINT16_LE ((guint8 *) context->codec_priv + 10, delay);
      }

      tmp =
          gst_buffer_new_wrapped (g_memdup (context->codec_priv,
              context->codec_priv_size), context->codec_priv_size);
      caps = gst_codec_utils_opus_create_caps_from_header (tmp, NULL);
      gst_buffer_unref (tmp);
      *codec_name = g_strdup ("Opus");
    } else if (context->codec_priv_size == 0) {
      GST_WARNING ("No Opus codec data found, trying to create one");
      if (audiocontext->channels <= 2) {
        guint8 streams, coupled, channels;
        guint32 samplerate;

        samplerate =
            audiocontext->samplerate == 0 ? 48000 : audiocontext->samplerate;
        rate = samplerate;
        channels = audiocontext->channels == 0 ? 2 : audiocontext->channels;
        if (channels == 1) {
          streams = 1;
          coupled = 0;
        } else {
          streams = 1;
          coupled = 1;
        }

        caps =
            gst_codec_utils_opus_create_caps (samplerate, channels, 0, streams,
            coupled, NULL);
        if (caps) {
          *codec_name = g_strdup ("Opus");
        } else {
          GST_WARNING ("Failed to create Opus caps from audio context");
        }
      } else {
        GST_WARNING ("No Opus codec data, and not enough info to create one");
      }
    } else {
      GST_WARNING ("Invalid Opus codec data size (got %" G_GSIZE_FORMAT
          ", expected 19)", context->codec_priv_size);
    }
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_ACM)) {
    gst_riff_strf_auds auds;

    if (data && size >= 18) {
      GstBuffer *codec_data = NULL;

      /* little-endian -> byte-order */
      auds.format = GST_READ_UINT16_LE (data);
      auds.channels = GST_READ_UINT16_LE (data + 2);
      auds.rate = GST_READ_UINT32_LE (data + 4);
      auds.av_bps = GST_READ_UINT32_LE (data + 8);
      auds.blockalign = GST_READ_UINT16_LE (data + 12);
      auds.bits_per_sample = GST_READ_UINT16_LE (data + 16);

      /* 18 is the waveformatex size */
      if (size > 18) {
        codec_data = gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY,
            data + 18, size - 18, 0, size - 18, NULL, NULL);
      }

      if (riff_audio_fmt)
        *riff_audio_fmt = auds.format;

      /* FIXME: Handle reorder map */
      caps = gst_riff_create_audio_caps (auds.format, NULL, &auds, codec_data,
          NULL, codec_name, NULL);
      if (codec_data)
        gst_buffer_unref (codec_data);

      if (caps == NULL) {
        GST_WARNING ("Unhandled RIFF audio format 0x%02x", auds.format);
      }
    } else {
      GST_WARNING ("Invalid codec data size (%d expected, got %d)", 18, size);
    }
  } else if (g_str_has_prefix (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_AAC)) {
    GstBuffer *priv = NULL;
    gint mpegversion;
    gint rate_idx, profile;
    guint8 *data = NULL;

    /* unspecified AAC profile with opaque private codec data */
    if (strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_AAC) == 0) {
      if (context->codec_priv_size >= 2) {
        guint obj_type, freq_index, explicit_freq_bytes = 0;

        codec_id = GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG4;
        mpegversion = 4;
        freq_index = (GST_READ_UINT16_BE (context->codec_priv) & 0x780) >> 7;
        obj_type = (GST_READ_UINT16_BE (context->codec_priv) & 0xF800) >> 11;
        if (freq_index == 15)
          explicit_freq_bytes = 3;
        GST_DEBUG ("obj_type = %u, freq_index = %u", obj_type, freq_index);
        priv = gst_buffer_new_wrapped (g_memdup (context->codec_priv,
                context->codec_priv_size), context->codec_priv_size);
        /* assume SBR if samplerate <= 24kHz */
        if (obj_type == 5 || (freq_index >= 6 && freq_index != 15) ||
            (context->codec_priv_size == (5 + explicit_freq_bytes))) {
          /* TODO: Commonly aacparse will reset the rate in caps to
           * non-multiplied - which one is correct? */
          audiocontext->samplerate *= 2;
        }
      } else {
        GST_WARNING ("Opaque A_AAC codec ID, but no codec private data");
        /* this is pretty broken;
         * maybe we need to make up some default private,
         * or maybe ADTS data got dumped in.
         * Let's set up some private data now, and check actual data later */
        /* just try this and see what happens ... */
        codec_id = GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG4;
        context->postprocess_frame = gst_matroska_demux_check_aac;
      }
    }

    /* make up decoder-specific data if it is not supplied */
    if (priv == NULL) {
      GstMapInfo map;

      priv = gst_buffer_new_allocate (NULL, 5, NULL);
      gst_buffer_map (priv, &map, GST_MAP_WRITE);
      data = map.data;
      rate_idx = aac_rate_idx (audiocontext->samplerate);
      profile = aac_profile_idx (codec_id);

      data[0] = ((profile + 1) << 3) | ((rate_idx & 0xE) >> 1);
      data[1] = ((rate_idx & 0x1) << 7) | (audiocontext->channels << 3);

      if (!strncmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG2,
              strlen (GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG2))) {
        mpegversion = 2;
        gst_buffer_unmap (priv, &map);
        gst_buffer_set_size (priv, 2);
      } else if (!strncmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG4,
              strlen (GST_MATROSKA_CODEC_ID_AUDIO_AAC_MPEG4))) {
        mpegversion = 4;

        if (g_strrstr (codec_id, "SBR")) {
          /* HE-AAC (aka SBR AAC) */
          audiocontext->samplerate *= 2;
          rate_idx = aac_rate_idx (audiocontext->samplerate);
          data[2] = AAC_SYNC_EXTENSION_TYPE >> 3;
          data[3] = ((AAC_SYNC_EXTENSION_TYPE & 0x07) << 5) | 5;
          data[4] = (1 << 7) | (rate_idx << 3);
          gst_buffer_unmap (priv, &map);
        } else {
          gst_buffer_unmap (priv, &map);
          gst_buffer_set_size (priv, 2);
        }
      } else {
        gst_buffer_unmap (priv, &map);
        gst_buffer_unref (priv);
        priv = NULL;
        GST_ERROR ("Unknown AAC profile and no codec private data");
      }
    }

    if (priv) {
      lead_in = 2;
      max_blocksize = 1024;
      caps = gst_caps_new_simple ("audio/mpeg",
          "mpegversion", G_TYPE_INT, mpegversion,
          "framed", G_TYPE_BOOLEAN, TRUE,
          "stream-format", G_TYPE_STRING, "raw", NULL);
      gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
      if (context->codec_priv && context->codec_priv_size > 0)
        gst_codec_utils_aac_caps_set_level_and_profile (caps,
            context->codec_priv, context->codec_priv_size);
      *codec_name = g_strdup_printf ("MPEG-%d AAC audio", mpegversion);
      gst_buffer_unref (priv);
    }
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_TTA)) {
    caps = gst_caps_new_simple ("audio/x-tta",
        "width", G_TYPE_INT, audiocontext->bitdepth, NULL);
    *codec_name = g_strdup ("TTA audio");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_WAVPACK4)) {
    caps = gst_caps_new_simple ("audio/x-wavpack",
        "width", G_TYPE_INT, audiocontext->bitdepth,
        "framed", G_TYPE_BOOLEAN, TRUE, NULL);
    *codec_name = g_strdup ("Wavpack audio");
    context->postprocess_frame = gst_matroska_demux_add_wvpk_header;
    audiocontext->wvpk_block_index = 0;
  } else if ((!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_14_4)) ||
      (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_28_8)) ||
      (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_COOK))) {
    gint raversion = -1;

    if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_14_4))
      raversion = 1;
    else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_COOK))
      raversion = 8;
    else
      raversion = 2;

    caps = gst_caps_new_simple ("audio/x-pn-realaudio",
        "raversion", G_TYPE_INT, raversion, NULL);
    /* Extract extra information from caps, mapping varies based on codec */
    if (data && (size >= 0x50)) {
      GstBuffer *priv;
      guint flavor;
      guint packet_size;
      guint height;
      guint leaf_size;
      guint sample_width;
      guint extra_data_size;

      GST_DEBUG ("real audio raversion:%d", raversion);
      if (raversion == 8) {
        /* COOK */
        flavor = GST_READ_UINT16_BE (data + 22);
        packet_size = GST_READ_UINT32_BE (data + 24);
        height = GST_READ_UINT16_BE (data + 40);
        leaf_size = GST_READ_UINT16_BE (data + 44);
        sample_width = GST_READ_UINT16_BE (data + 58);
        extra_data_size = GST_READ_UINT32_BE (data + 74);

        GST_DEBUG
            ("flavor:%d, packet_size:%d, height:%d, leaf_size:%d, sample_width:%d, extra_data_size:%d",
            flavor, packet_size, height, leaf_size, sample_width,
            extra_data_size);
        gst_caps_set_simple (caps, "flavor", G_TYPE_INT, flavor, "packet_size",
            G_TYPE_INT, packet_size, "height", G_TYPE_INT, height, "leaf_size",
            G_TYPE_INT, leaf_size, "width", G_TYPE_INT, sample_width, NULL);

        if ((size - 78) >= extra_data_size) {
          priv = gst_buffer_new_wrapped (g_memdup (data + 78, extra_data_size),
              extra_data_size);
          gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, priv, NULL);
          gst_buffer_unref (priv);
        }
      }
    }

    *codec_name = g_strdup_printf ("RealAudio %d.0", raversion);
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_SIPR)) {
    caps = gst_caps_new_empty_simple ("audio/x-sipro");
    *codec_name = g_strdup ("Sipro/ACELP.NET Voice Codec");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_RALF)) {
    caps = gst_caps_new_empty_simple ("audio/x-ralf-mpeg4-generic");
    *codec_name = g_strdup ("Real Audio Lossless");
  } else if (!strcmp (codec_id, GST_MATROSKA_CODEC_ID_AUDIO_REAL_ATRC)) {
    caps = gst_caps_new_empty_simple ("audio/x-vnd.sony.atrac3");
    *codec_name = g_strdup ("Sony ATRAC3");
  } else {
    GST_WARNING ("Unknown codec '%s', cannot build Caps", codec_id);
    return NULL;
  }

  if (caps != NULL) {
    if (audiocontext->samplerate > 0 && audiocontext->channels > 0) {
      gint i;

      for (i = 0; i < gst_caps_get_size (caps); i++) {
        gst_structure_set (gst_caps_get_structure (caps, i),
            "channels", G_TYPE_INT, audiocontext->channels,
            "rate", G_TYPE_INT, audiocontext->samplerate, NULL);
      }
    }

    caps = gst_caps_simplify (caps);
  }

  if (lead_in_ts && lead_in && max_blocksize && rate) {
    *lead_in_ts =
        gst_util_uint64_scale (GST_SECOND, max_blocksize * lead_in, rate);
  }

  return caps;
}