qtdemux_parse_trak (GstQTDemux * qtdemux, GNode * trak)
{
  GstByteReader tkhd;
  int offset;
  GNode *mdia;
  GNode *mdhd;
  GNode *hdlr;
  GNode *minf;
  GNode *stbl;
  GNode *stsd;
  GNode *mp4a;
  GNode *mp4v;
  GNode *wave;
  GNode *esds;
  GNode *pasp;
  GNode *colr;
  GNode *tref;
  GNode *udta;
  GNode *svmi;
  GNode *fiel;

  QtDemuxStream *stream = NULL;
  gboolean new_stream = FALSE;
  gchar *codec = NULL;
  const guint8 *stsd_data;
  guint16 lang_code;            /* quicktime lang code or packed iso code */
  guint32 version;
  guint32 tkhd_flags = 0;
  guint8 tkhd_version = 0;
  guint32 fourcc;
  guint value_size, stsd_len, len;
  guint32 track_id;
  guint32 dummy;

  GST_DEBUG_OBJECT (qtdemux, "parse_trak");

  if (!qtdemux_tree_get_child_by_type_full (trak, FOURCC_tkhd, &tkhd)
      || !gst_byte_reader_get_uint8 (&tkhd, &tkhd_version)
      || !gst_byte_reader_get_uint24_be (&tkhd, &tkhd_flags))
    goto corrupt_file;

  /* pick between 64 or 32 bits */
  value_size = tkhd_version == 1 ? 8 : 4;
  if (!gst_byte_reader_skip (&tkhd, value_size * 2) ||
      !gst_byte_reader_get_uint32_be (&tkhd, &track_id))
    goto corrupt_file;

  if (!qtdemux->got_moov) {
    if (qtdemux_find_stream (qtdemux, track_id))
      goto existing_stream;
    stream = _create_stream ();
    stream->track_id = track_id;
    new_stream = TRUE;
  } else {
    stream = qtdemux_find_stream (qtdemux, track_id);
    if (!stream) {
      GST_WARNING_OBJECT (qtdemux, "Stream not found, going to ignore it");
      goto skip_track;
    }

    /* flush samples data from this track from previous moov */
    gst_qtdemux_stream_flush_segments_data (qtdemux, stream);
    gst_qtdemux_stream_flush_samples_data (qtdemux, stream);
  }
  /* need defaults for fragments */
  qtdemux_parse_trex (qtdemux, stream, &dummy, &dummy, &dummy);

  if (stream->pending_tags == NULL)
    stream->pending_tags = gst_tag_list_new_empty ();

  if ((tkhd_flags & 1) == 0)
    stream->disabled = TRUE;

  GST_LOG_OBJECT (qtdemux, "track[tkhd] version/flags/id: 0x%02x/%06x/%u",
      tkhd_version, tkhd_flags, stream->track_id);

  if (!(mdia = qtdemux_tree_get_child_by_type (trak, FOURCC_mdia)))
    goto corrupt_file;

  if (!(mdhd = qtdemux_tree_get_child_by_type (mdia, FOURCC_mdhd))) {
    /* be nice for some crooked mjp2 files that use mhdr for mdhd */
    if (qtdemux->major_brand != FOURCC_mjp2 ||
        !(mdhd = qtdemux_tree_get_child_by_type (mdia, FOURCC_mhdr)))
      goto corrupt_file;
  }

  len = QT_UINT32 ((guint8 *) mdhd->data);
  version = QT_UINT32 ((guint8 *) mdhd->data + 8);
  GST_LOG_OBJECT (qtdemux, "track version/flags: %08x", version);
  if (version == 0x01000000) {
    if (len < 38)
      goto corrupt_file;
    stream->timescale = QT_UINT32 ((guint8 *) mdhd->data + 28);
    stream->duration = QT_UINT64 ((guint8 *) mdhd->data + 32);
    lang_code = QT_UINT16 ((guint8 *) mdhd->data + 36);
  } else {
    if (len < 30)
      goto corrupt_file;
    stream->timescale = QT_UINT32 ((guint8 *) mdhd->data + 20);
    stream->duration = QT_UINT32 ((guint8 *) mdhd->data + 24);
    lang_code = QT_UINT16 ((guint8 *) mdhd->data + 28);
  }

  if (lang_code < 0x400) {
    qtdemux_lang_map_qt_code_to_iso (stream->lang_id, lang_code);
  } else if (lang_code == 0x7fff) {
    stream->lang_id[0] = 0;     /* unspecified */
  } else {
    stream->lang_id[0] = 0x60 + ((lang_code >> 10) & 0x1F);
    stream->lang_id[1] = 0x60 + ((lang_code >> 5) & 0x1F);
    stream->lang_id[2] = 0x60 + (lang_code & 0x1F);
    stream->lang_id[3] = 0;
  }

  GST_LOG_OBJECT (qtdemux, "track timescale: %" G_GUINT32_FORMAT,
      stream->timescale);
  GST_LOG_OBJECT (qtdemux, "track duration: %" G_GUINT64_FORMAT,
      stream->duration);
  GST_LOG_OBJECT (qtdemux, "track language code/id: 0x%04x/%s",
      lang_code, stream->lang_id);

  if (G_UNLIKELY (stream->timescale == 0 || qtdemux->timescale == 0))
    goto corrupt_file;

  if ((tref = qtdemux_tree_get_child_by_type (trak, FOURCC_tref))) {
    /* chapters track reference */
    GNode *chap = qtdemux_tree_get_child_by_type (tref, FOURCC_chap);
    if (chap) {
      gsize length = GST_READ_UINT32_BE (chap->data);
      if (qtdemux->chapters_track_id)
        GST_FIXME_OBJECT (qtdemux, "Multiple CHAP tracks");

      if (length >= 12) {
        qtdemux->chapters_track_id =
            GST_READ_UINT32_BE ((gint8 *) chap->data + 8);
      }
    }
  }

  /* fragmented files may have bogus duration in moov */
  if (!qtdemux->fragmented &&
      qtdemux->duration != G_MAXINT64 && stream->duration != G_MAXINT32) {
    guint64 tdur1, tdur2;

    /* don't overflow */
    tdur1 = stream->timescale * (guint64) qtdemux->duration;
    tdur2 = qtdemux->timescale * (guint64) stream->duration;

    /* HACK:
     * some of those trailers, nowadays, have prologue images that are
     * themselves video tracks as well. I haven't really found a way to
     * identify those yet, except for just looking at their duration. */
    if (tdur1 != 0 && (tdur2 * 10 / tdur1) < 2) {
      GST_WARNING_OBJECT (qtdemux,
          "Track shorter than 20%% (%" G_GUINT64_FORMAT "/%" G_GUINT32_FORMAT
          " vs. %" G_GUINT64_FORMAT "/%" G_GUINT32_FORMAT ") of the stream "
          "found, assuming preview image or something; skipping track",
          stream->duration, stream->timescale, qtdemux->duration,
          qtdemux->timescale);
      if (new_stream)
        gst_qtdemux_stream_free (qtdemux, stream);
      return TRUE;
    }
  }

  if (!(hdlr = qtdemux_tree_get_child_by_type (mdia, FOURCC_hdlr)))
    goto corrupt_file;

  GST_LOG_OBJECT (qtdemux, "track type: %" GST_FOURCC_FORMAT,
      GST_FOURCC_ARGS (QT_FOURCC ((guint8 *) hdlr->data + 12)));

  len = QT_UINT32 ((guint8 *) hdlr->data);
  if (len >= 20)
    stream->subtype = QT_FOURCC ((guint8 *) hdlr->data + 16);
  GST_LOG_OBJECT (qtdemux, "track subtype: %" GST_FOURCC_FORMAT,
      GST_FOURCC_ARGS (stream->subtype));

  if (!(minf = qtdemux_tree_get_child_by_type (mdia, FOURCC_minf)))
    goto corrupt_file;

  if (!(stbl = qtdemux_tree_get_child_by_type (minf, FOURCC_stbl)))
    goto corrupt_file;

  /*parse svmi header if existing */
  svmi = qtdemux_tree_get_child_by_type (stbl, FOURCC_svmi);
  if (svmi) {
    len = QT_UINT32 ((guint8 *) svmi->data);
    version = QT_UINT32 ((guint8 *) svmi->data + 8);
    if (!version) {
      GstVideoMultiviewMode mode = GST_VIDEO_MULTIVIEW_MODE_NONE;
      GstVideoMultiviewFlags flags = GST_VIDEO_MULTIVIEW_FLAGS_NONE;
      guint8 frame_type, frame_layout;

      /* MPEG-A stereo video */
      if (qtdemux->major_brand == FOURCC_ss02)
        flags |= GST_VIDEO_MULTIVIEW_FLAGS_MIXED_MONO;

      frame_type = QT_UINT8 ((guint8 *) svmi->data + 12);
      frame_layout = QT_UINT8 ((guint8 *) svmi->data + 13) & 0x01;
      switch (frame_type) {
        case 0:
          mode = GST_VIDEO_MULTIVIEW_MODE_SIDE_BY_SIDE;
          break;
        case 1:
          mode = GST_VIDEO_MULTIVIEW_MODE_ROW_INTERLEAVED;
          break;
        case 2:
          mode = GST_VIDEO_MULTIVIEW_MODE_FRAME_BY_FRAME;
          break;
        case 3:
          /* mode 3 is primary/secondary view sequence, ie
           * left/right views in separate tracks. See section 7.2
           * of ISO/IEC 23000-11:2009 */
          GST_FIXME_OBJECT (qtdemux,
              "Implement stereo video in separate streams");
      }

      if ((frame_layout & 0x1) == 0)
        flags |= GST_VIDEO_MULTIVIEW_FLAGS_RIGHT_VIEW_FIRST;

      GST_LOG_OBJECT (qtdemux,
          "StereoVideo: composition type: %u, is_left_first: %u",
          frame_type, frame_layout);
      stream->multiview_mode = mode;
      stream->multiview_flags = flags;
    }
  }

  /* parse stsd */
  if (!(stsd = qtdemux_tree_get_child_by_type (stbl, FOURCC_stsd)))
    goto corrupt_file;
  stsd_data = (const guint8 *) stsd->data;

  /* stsd should at least have one entry */
  stsd_len = QT_UINT32 (stsd_data);
  if (stsd_len < 24) {
    /* .. but skip stream with empty stsd produced by some Vivotek cameras */
    if (stream->subtype == FOURCC_vivo) {
      if (new_stream)
        gst_qtdemux_stream_free (qtdemux, stream);
      return TRUE;
    } else {
      goto corrupt_file;
    }
  }

  GST_LOG_OBJECT (qtdemux, "stsd len:           %d", stsd_len);

  /* and that entry should fit within stsd */
  len = QT_UINT32 (stsd_data + 16);
  if (len > stsd_len + 16)
    goto corrupt_file;

  stream->fourcc = fourcc = QT_FOURCC (stsd_data + 16 + 4);
  GST_LOG_OBJECT (qtdemux, "stsd type:          %" GST_FOURCC_FORMAT,
      GST_FOURCC_ARGS (stream->fourcc));
  GST_LOG_OBJECT (qtdemux, "stsd type len:      %d", len);

  if ((fourcc == FOURCC_drms) || (fourcc == FOURCC_drmi))
    goto error_encrypted;

  if (fourcc == FOURCC_encv || fourcc == FOURCC_enca) {
    GNode *enc = qtdemux_tree_get_child_by_type (stsd, fourcc);
    stream->protected = TRUE;
    if (!qtdemux_parse_protection_scheme_info (qtdemux, stream, enc, &fourcc))
      GST_ERROR_OBJECT (qtdemux, "Failed to parse protection scheme info");
  }

  if (stream->subtype == FOURCC_vide) {
    guint32 w = 0, h = 0;
    gboolean gray;
    gint depth, palette_size, palette_count;
    guint32 matrix[9];
    guint32 *palette_data = NULL;

    stream->sampled = TRUE;

    /* version 1 uses some 64-bit ints */
    if (!gst_byte_reader_skip (&tkhd, 20 + value_size))
      goto corrupt_file;

    if (!qtdemux_parse_transformation_matrix (qtdemux, &tkhd, matrix, "tkhd"))
      goto corrupt_file;

    if (!gst_byte_reader_get_uint32_be (&tkhd, &w)
        || !gst_byte_reader_get_uint32_be (&tkhd, &h))
      goto corrupt_file;

    stream->display_width = w >> 16;
    stream->display_height = h >> 16;

    qtdemux_inspect_transformation_matrix (qtdemux, stream, matrix,
        &stream->pending_tags);

    offset = 16;
    if (len < 86)
      goto corrupt_file;

    stream->width = QT_UINT16 (stsd_data + offset + 32);
    stream->height = QT_UINT16 (stsd_data + offset + 34);
    stream->fps_n = 0;          /* this is filled in later */
    stream->fps_d = 0;          /* this is filled in later */
    stream->bits_per_sample = QT_UINT16 (stsd_data + offset + 82);
    stream->color_table_id = QT_UINT16 (stsd_data + offset + 84);

    /* if color_table_id is 0, ctab atom must follow; however some files
     * produced by TMPEGEnc have color_table_id = 0 and no ctab atom, so
     * if color table is not present we'll correct the value */
    if (stream->color_table_id == 0 &&
        (len < 90 || QT_FOURCC (stsd_data + offset + 86) != FOURCC_ctab)) {
      stream->color_table_id = -1;
    }

    GST_LOG_OBJECT (qtdemux, "width %d, height %d, bps %d, color table id %d",
        stream->width, stream->height, stream->bits_per_sample,
        stream->color_table_id);

    depth = stream->bits_per_sample;

    /* more than 32 bits means grayscale */
    gray = (depth > 32);
    /* low 32 bits specify the depth  */
    depth &= 0x1F;

    /* different number of palette entries is determined by depth. */
    palette_count = 0;
    if ((depth == 1) || (depth == 2) || (depth == 4) || (depth == 8))
      palette_count = (1 << depth);
    palette_size = palette_count * 4;

    if (stream->color_table_id) {
      switch (palette_count) {
        case 0:
          break;
        case 2:
          palette_data = g_memdup (ff_qt_default_palette_2, palette_size);
          break;
        case 4:
          palette_data = g_memdup (ff_qt_default_palette_4, palette_size);
          break;
        case 16:
          if (gray)
            palette_data = g_memdup (ff_qt_grayscale_palette_16, palette_size);
          else
            palette_data = g_memdup (ff_qt_default_palette_16, palette_size);
          break;
        case 256:
          if (gray)
            palette_data = g_memdup (ff_qt_grayscale_palette_256, palette_size);
          else
            palette_data = g_memdup (ff_qt_default_palette_256, palette_size);
          break;
        default:
          GST_ELEMENT_WARNING (qtdemux, STREAM, DEMUX,
              (_("The video in this file might not play correctly.")),
              ("unsupported palette depth %d", depth));
          break;
      }
    } else {
      gint i, j, start, end;

      if (len < 94)
        goto corrupt_file;

      /* read table */
      start = QT_UINT32 (stsd_data + offset + 86);
      palette_count = QT_UINT16 (stsd_data + offset + 90);
      end = QT_UINT16 (stsd_data + offset + 92);

      GST_LOG_OBJECT (qtdemux, "start %d, end %d, palette_count %d",
          start, end, palette_count);

      if (end > 255)
        end = 255;
      if (start > end)
        start = end;

      if (len < 94 + (end - start) * 8)
        goto corrupt_file;

      /* palette is always the same size */
      palette_data = g_malloc0 (256 * 4);
      palette_size = 256 * 4;

      for (j = 0, i = start; i <= end; j++, i++) {
        guint32 a, r, g, b;

        a = QT_UINT16 (stsd_data + offset + 94 + (j * 8));
        r = QT_UINT16 (stsd_data + offset + 96 + (j * 8));
        g = QT_UINT16 (stsd_data + offset + 98 + (j * 8));
        b = QT_UINT16 (stsd_data + offset + 100 + (j * 8));

        palette_data[i] = ((a & 0xff00) << 16) | ((r & 0xff00) << 8) |
            (g & 0xff00) | (b >> 8);
      }
    }

    if (stream->caps)
      gst_caps_unref (stream->caps);

    stream->caps =
        qtdemux_video_caps (qtdemux, stream, fourcc, stsd_data, &codec);
    if (G_UNLIKELY (!stream->caps)) {
      g_free (palette_data);
      goto unknown_stream;
    }

    if (codec) {
      gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
          GST_TAG_VIDEO_CODEC, codec, NULL);
      g_free (codec);
      codec = NULL;
    }


    if (palette_data) {
      GstStructure *s;

      if (stream->rgb8_palette)
        gst_memory_unref (stream->rgb8_palette);
      stream->rgb8_palette = gst_memory_new_wrapped (GST_MEMORY_FLAG_READONLY,
          palette_data, palette_size, 0, palette_size, palette_data, g_free);

      s = gst_caps_get_structure (stream->caps, 0);

      /* non-raw video has a palette_data property. raw video has the palette as
       * an extra plane that we append to the output buffers before we push
       * them*/
      if (!gst_structure_has_name (s, "video/x-raw")) {
        GstBuffer *palette;

        palette = gst_buffer_new ();
        gst_buffer_append_memory (palette, stream->rgb8_palette);
        stream->rgb8_palette = NULL;

        gst_caps_set_simple (stream->caps, "palette_data",
            GST_TYPE_BUFFER, palette, NULL);
        gst_buffer_unref (palette);
      }
    } else if (palette_count != 0) {
      GST_ELEMENT_WARNING (qtdemux, STREAM, NOT_IMPLEMENTED,
          (NULL), ("Unsupported palette depth %d", depth));
    }

    GST_LOG_OBJECT (qtdemux, "frame count:   %u",
        QT_UINT16 (stsd_data + offset + 48));

    esds = NULL;
    pasp = NULL;
    colr = NULL;
    fiel = NULL;
    /* pick 'the' stsd child */
    if (!stream->protected)
      mp4v = qtdemux_tree_get_child_by_type (stsd, fourcc);
    else
      mp4v = qtdemux_tree_get_child_by_type (stsd, FOURCC_encv);

    if (mp4v) {
      esds = qtdemux_tree_get_child_by_type (mp4v, FOURCC_esds);
      pasp = qtdemux_tree_get_child_by_type (mp4v, FOURCC_pasp);
      colr = qtdemux_tree_get_child_by_type (mp4v, FOURCC_colr);
      fiel = qtdemux_tree_get_child_by_type (mp4v, FOURCC_fiel);
    }

    if (pasp) {
      const guint8 *pasp_data = (const guint8 *) pasp->data;

      stream->par_w = QT_UINT32 (pasp_data + 8);
      stream->par_h = QT_UINT32 (pasp_data + 12);
    } else {
      stream->par_w = 0;
      stream->par_h = 0;
    }

    if (fiel) {
      const guint8 *fiel_data = (const guint8 *) fiel->data;
      gint len = QT_UINT32 (fiel_data);

      if (len == 10) {
        stream->interlace_mode = GST_READ_UINT8 (fiel_data + 8);
        stream->field_order = GST_READ_UINT8 (fiel_data + 9);
      }
    }

    if (colr) {
      const guint8 *colr_data = (const guint8 *) colr->data;
      gint len = QT_UINT32 (colr_data);

      if (len == 19 || len == 18) {
        guint32 color_type = GST_READ_UINT32_LE (colr_data + 8);

        if (color_type == FOURCC_nclx || color_type == FOURCC_nclc) {
          guint16 primaries = GST_READ_UINT16_BE (colr_data + 12);
          guint16 transfer_function = GST_READ_UINT16_BE (colr_data + 14);
          guint16 matrix = GST_READ_UINT16_BE (colr_data + 16);
          gboolean full_range = len == 19 ? colr_data[17] >> 7 : FALSE;

          switch (primaries) {
            case 1:
              stream->colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_BT709;
              break;
            case 5:
              stream->colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_BT470BG;
              break;
            case 6:
              stream->colorimetry.primaries =
                  GST_VIDEO_COLOR_PRIMARIES_SMPTE170M;
              break;
            case 9:
              stream->colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_BT2020;
              break;
            default:
              break;
          }

          switch (transfer_function) {
            case 1:
              stream->colorimetry.transfer = GST_VIDEO_TRANSFER_BT709;
              break;
            case 7:
              stream->colorimetry.transfer = GST_VIDEO_TRANSFER_SMPTE240M;
              break;
            default:
              break;
          }

          switch (matrix) {
            case 1:
              stream->colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_BT709;
              break;
            case 6:
              stream->colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_BT601;
              break;
            case 7:
              stream->colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_SMPTE240M;
              break;
            case 9:
              stream->colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_BT2020;
              break;
            default:
              break;
          }

          stream->colorimetry.range =
              full_range ? GST_VIDEO_COLOR_RANGE_0_255 :
              GST_VIDEO_COLOR_RANGE_16_235;
        } else {
          GST_DEBUG_OBJECT (qtdemux, "Unsupported color type");
        }
      } else {
        GST_WARNING_OBJECT (qtdemux, "Invalid colr atom size");
      }
    }

    if (esds) {
      gst_qtdemux_handle_esds (qtdemux, stream, esds, stream->pending_tags);
    } else {
      switch (fourcc) {
        case FOURCC_H264:
        case FOURCC_avc1:
        case FOURCC_avc3:
        {
          gint len = QT_UINT32 (stsd_data) - 0x66;
          const guint8 *avc_data = stsd_data + 0x66;

          /* find avcC */
          while (len >= 0x8) {
            gint size;

            if (QT_UINT32 (avc_data) <= len)
              size = QT_UINT32 (avc_data) - 0x8;
            else
              size = len - 0x8;

            if (size < 1)
              /* No real data, so break out */
              break;

            switch (QT_FOURCC (avc_data + 0x4)) {
              case FOURCC_avcC:
              {
                /* parse, if found */
                GstBuffer *buf;

                GST_DEBUG_OBJECT (qtdemux, "found avcC codec_data in stsd");

                /* First 4 bytes are the length of the atom, the next 4 bytes
                 * are the fourcc, the next 1 byte is the version, and the
                 * subsequent bytes are profile_tier_level structure like data. */
                gst_codec_utils_h264_caps_set_level_and_profile (stream->caps,
                    avc_data + 8 + 1, size - 1);
                buf = gst_buffer_new_and_alloc (size);
                gst_buffer_fill (buf, 0, avc_data + 0x8, size);
                gst_caps_set_simple (stream->caps,
                    "codec_data", GST_TYPE_BUFFER, buf, NULL);
                gst_buffer_unref (buf);

                break;
              }
              case FOURCC_strf:
              {
                GstBuffer *buf;

                GST_DEBUG_OBJECT (qtdemux, "found strf codec_data in stsd");

                /* First 4 bytes are the length of the atom, the next 4 bytes
                 * are the fourcc, next 40 bytes are BITMAPINFOHEADER,
                 * next 1 byte is the version, and the
                 * subsequent bytes are sequence parameter set like data. */

                size -= 40;     /* we'll be skipping BITMAPINFOHEADER */
                if (size > 1) {
                  gst_codec_utils_h264_caps_set_level_and_profile (stream->caps,
                      avc_data + 8 + 40 + 1, size - 1);

                  buf = gst_buffer_new_and_alloc (size);
                  gst_buffer_fill (buf, 0, avc_data + 8 + 40, size);
                  gst_caps_set_simple (stream->caps,
                      "codec_data", GST_TYPE_BUFFER, buf, NULL);
                  gst_buffer_unref (buf);
                }
                break;
              }
              case FOURCC_btrt:
              {
                guint avg_bitrate, max_bitrate;

                /* bufferSizeDB, maxBitrate and avgBitrate - 4 bytes each */
                if (size < 12)
                  break;

                max_bitrate = QT_UINT32 (avc_data + 0xc);
                avg_bitrate = QT_UINT32 (avc_data + 0x10);

                if (!max_bitrate && !avg_bitrate)
                  break;

                /* Some muxers seem to swap the average and maximum bitrates
                 * (I'm looking at you, YouTube), so we swap for sanity. */
                if (max_bitrate > 0 && max_bitrate < avg_bitrate) {
                  guint temp = avg_bitrate;

                  avg_bitrate = max_bitrate;
                  max_bitrate = temp;
                }

                if (max_bitrate > 0 && max_bitrate < G_MAXUINT32) {
                  gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
                      GST_TAG_MAXIMUM_BITRATE, max_bitrate, NULL);
                }
                if (avg_bitrate > 0 && avg_bitrate < G_MAXUINT32) {
                  gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
                      GST_TAG_BITRATE, avg_bitrate, NULL);
                }

                break;
              }

              default:
                break;
            }

            len -= size + 8;
            avc_data += size + 8;
          }

          break;
        }
        case FOURCC_H265:
        case FOURCC_hvc1:
        case FOURCC_hev1:
        {
          gint len = QT_UINT32 (stsd_data) - 0x66;
          const guint8 *hevc_data = stsd_data + 0x66;

          /* find hevc */
          while (len >= 0x8) {
            gint size;

            if (QT_UINT32 (hevc_data) <= len)
              size = QT_UINT32 (hevc_data) - 0x8;
            else
              size = len - 0x8;

            if (size < 1)
              /* No real data, so break out */
              break;

            switch (QT_FOURCC (hevc_data + 0x4)) {
              case FOURCC_hvcC:
              {
                /* parse, if found */
                GstBuffer *buf;

                GST_DEBUG_OBJECT (qtdemux, "found avcC codec_data in stsd");

                /* First 4 bytes are the length of the atom, the next 4 bytes
                 * are the fourcc, the next 1 byte is the version, and the
                 * subsequent bytes are sequence parameter set like data. */
                gst_codec_utils_h265_caps_set_level_tier_and_profile
                    (stream->caps, hevc_data + 8 + 1, size - 1);

                buf = gst_buffer_new_and_alloc (size);
                gst_buffer_fill (buf, 0, hevc_data + 0x8, size);
                gst_caps_set_simple (stream->caps,
                    "codec_data", GST_TYPE_BUFFER, buf, NULL);
                gst_buffer_unref (buf);
                break;
              }
              default:
                break;
            }
            len -= size + 8;
            hevc_data += size + 8;
          }
          break;
        }
        case FOURCC_mp4v:
        case FOURCC_MP4V:
        case FOURCC_fmp4:
        case FOURCC_FMP4:
        {
          GNode *glbl;

          GST_DEBUG_OBJECT (qtdemux, "found %" GST_FOURCC_FORMAT,
              GST_FOURCC_ARGS (fourcc));

          /* codec data might be in glbl extension atom */
          glbl = mp4v ?
              qtdemux_tree_get_child_by_type (mp4v, FOURCC_glbl) : NULL;
          if (glbl) {
            guint8 *data;
            GstBuffer *buf;
            gint len;

            GST_DEBUG_OBJECT (qtdemux, "found glbl data in stsd");
            data = glbl->data;
            len = QT_UINT32 (data);
            if (len > 0x8) {
              len -= 0x8;
              buf = gst_buffer_new_and_alloc (len);
              gst_buffer_fill (buf, 0, data + 8, len);
              gst_caps_set_simple (stream->caps,
                  "codec_data", GST_TYPE_BUFFER, buf, NULL);
              gst_buffer_unref (buf);
            }
          }
          break;
        }
        case FOURCC_mjp2:
        {
          /* see annex I of the jpeg2000 spec */
          GNode *jp2h, *ihdr, *colr, *mjp2, *field, *prefix, *cmap, *cdef;
          const guint8 *data;
          const gchar *colorspace = NULL;
          gint ncomp = 0;
          guint32 ncomp_map = 0;
          gint32 *comp_map = NULL;
          guint32 nchan_def = 0;
          gint32 *chan_def = NULL;

          GST_DEBUG_OBJECT (qtdemux, "found mjp2");
          /* some required atoms */
          mjp2 = qtdemux_tree_get_child_by_type (stsd, FOURCC_mjp2);
          if (!mjp2)
            break;
          jp2h = qtdemux_tree_get_child_by_type (mjp2, FOURCC_jp2h);
          if (!jp2h)
            break;

          /* number of components; redundant with info in codestream, but useful
             to a muxer */
          ihdr = qtdemux_tree_get_child_by_type (jp2h, FOURCC_ihdr);
          if (!ihdr || QT_UINT32 (ihdr->data) != 22)
            break;
          ncomp = QT_UINT16 (((guint8 *) ihdr->data) + 16);

          colr = qtdemux_tree_get_child_by_type (jp2h, FOURCC_colr);
          if (!colr)
            break;
          GST_DEBUG_OBJECT (qtdemux, "found colr");
          /* extract colour space info */
          if (QT_UINT8 ((guint8 *) colr->data + 8) == 1) {
            switch (QT_UINT32 ((guint8 *) colr->data + 11)) {
              case 16:
                colorspace = "sRGB";
                break;
              case 17:
                colorspace = "GRAY";
                break;
              case 18:
                colorspace = "sYUV";
                break;
              default:
                colorspace = NULL;
                break;
            }
          }
          if (!colorspace)
            /* colr is required, and only values 16, 17, and 18 are specified,
               so error if we have no colorspace */
            break;

          /* extract component mapping */
          cmap = qtdemux_tree_get_child_by_type (jp2h, FOURCC_cmap);
          if (cmap) {
            guint32 cmap_len = 0;
            int i;
            cmap_len = QT_UINT32 (cmap->data);
            if (cmap_len >= 8) {
              /* normal box, subtract off header */
              cmap_len -= 8;
              /* cmap: { u16 cmp; u8 mtyp; u8 pcol; }* */
              if (cmap_len % 4 == 0) {
                ncomp_map = (cmap_len / 4);
                comp_map = g_new0 (gint32, ncomp_map);
                for (i = 0; i < ncomp_map; i++) {
                  guint16 cmp;
                  guint8 mtyp, pcol;
                  cmp = QT_UINT16 (((guint8 *) cmap->data) + 8 + i * 4);
                  mtyp = QT_UINT8 (((guint8 *) cmap->data) + 8 + i * 4 + 2);
                  pcol = QT_UINT8 (((guint8 *) cmap->data) + 8 + i * 4 + 3);
                  comp_map[i] = (mtyp << 24) | (pcol << 16) | cmp;
                }
              }
            }
          }
          /* extract channel definitions */
          cdef = qtdemux_tree_get_child_by_type (jp2h, FOURCC_cdef);
          if (cdef) {
            guint32 cdef_len = 0;
            int i;
            cdef_len = QT_UINT32 (cdef->data);
            if (cdef_len >= 10) {
              /* normal box, subtract off header and len */
              cdef_len -= 10;
              /* cdef: u16 n; { u16 cn; u16 typ; u16 asoc; }* */
              if (cdef_len % 6 == 0) {
                nchan_def = (cdef_len / 6);
                chan_def = g_new0 (gint32, nchan_def);
                for (i = 0; i < nchan_def; i++)
                  chan_def[i] = -1;
                for (i = 0; i < nchan_def; i++) {
                  guint16 cn, typ, asoc;
                  cn = QT_UINT16 (((guint8 *) cdef->data) + 10 + i * 6);
                  typ = QT_UINT16 (((guint8 *) cdef->data) + 10 + i * 6 + 2);
                  asoc = QT_UINT16 (((guint8 *) cdef->data) + 10 + i * 6 + 4);
                  if (cn < nchan_def) {
                    switch (typ) {
                      case 0:
                        chan_def[cn] = asoc;
                        break;
                      case 1:
                        chan_def[cn] = 0;       /* alpha */
                        break;
                      default:
                        chan_def[cn] = -typ;
                    }
                  }
                }
              }
            }
          }

          gst_caps_set_simple (stream->caps,
              "num-components", G_TYPE_INT, ncomp, NULL);
          gst_caps_set_simple (stream->caps,
              "colorspace", G_TYPE_STRING, colorspace, NULL);

          if (comp_map) {
            GValue arr = { 0, };
            GValue elt = { 0, };
            int i;
            g_value_init (&arr, GST_TYPE_ARRAY);
            g_value_init (&elt, G_TYPE_INT);
            for (i = 0; i < ncomp_map; i++) {
              g_value_set_int (&elt, comp_map[i]);
              gst_value_array_append_value (&arr, &elt);
            }
            gst_structure_set_value (gst_caps_get_structure (stream->caps, 0),
                "component-map", &arr);
            g_value_unset (&elt);
            g_value_unset (&arr);
            g_free (comp_map);
          }

          if (chan_def) {
            GValue arr = { 0, };
            GValue elt = { 0, };
            int i;
            g_value_init (&arr, GST_TYPE_ARRAY);
            g_value_init (&elt, G_TYPE_INT);
            for (i = 0; i < nchan_def; i++) {
              g_value_set_int (&elt, chan_def[i]);
              gst_value_array_append_value (&arr, &elt);
            }
            gst_structure_set_value (gst_caps_get_structure (stream->caps, 0),
                "channel-definitions", &arr);
            g_value_unset (&elt);
            g_value_unset (&arr);
            g_free (chan_def);
          }

          /* some optional atoms */
          field = qtdemux_tree_get_child_by_type (mjp2, FOURCC_fiel);
          prefix = qtdemux_tree_get_child_by_type (mjp2, FOURCC_jp2x);

          /* indicate possible fields in caps */
          if (field) {
            data = (guint8 *) field->data + 8;
            if (*data != 1)
              gst_caps_set_simple (stream->caps, "fields", G_TYPE_INT,
                  (gint) * data, NULL);
          }
          /* add codec_data if provided */
          if (prefix) {
            GstBuffer *buf;
            gint len;

            GST_DEBUG_OBJECT (qtdemux, "found prefix data in stsd");
            data = prefix->data;
            len = QT_UINT32 (data);
            if (len > 0x8) {
              len -= 0x8;
              buf = gst_buffer_new_and_alloc (len);
              gst_buffer_fill (buf, 0, data + 8, len);
              gst_caps_set_simple (stream->caps,
                  "codec_data", GST_TYPE_BUFFER, buf, NULL);
              gst_buffer_unref (buf);
            }
          }
          break;
        }
        case FOURCC_jpeg:
        {
          /* https://developer.apple.com/standards/qtff-2001.pdf,
           * page 92, "Video Sample Description", under table 3.1 */
          GstByteReader br;

          const gint compressor_offset = 16 + 4 + 4 * 3 + 2 * 2 + 2 * 4 + 4 + 2;
          const gint min_size = compressor_offset + 32 + 2 + 2;
          GNode *jpeg;
          guint32 len;
          guint16 color_table_id = 0;
          gboolean ok;

          GST_DEBUG_OBJECT (qtdemux, "found jpeg");

          /* recover information on interlaced/progressive */
          jpeg = qtdemux_tree_get_child_by_type (stsd, FOURCC_jpeg);
          if (!jpeg)
            break;

          len = QT_UINT32 (jpeg->data);
          GST_DEBUG_OBJECT (qtdemux, "Found jpeg: len %u, need %d", len,
              min_size);
          if (len >= min_size) {
            gst_byte_reader_init (&br, jpeg->data, len);

            gst_byte_reader_skip (&br, compressor_offset + 32 + 2);
            gst_byte_reader_get_uint16_le (&br, &color_table_id);
            if (color_table_id != 0) {
              /* the spec says there can be concatenated chunks in the data, and we want
               * to find one called field. Walk through them. */
              gint offset = min_size;
              while (offset + 8 < len) {
                guint32 size = 0, tag;
                ok = gst_byte_reader_get_uint32_le (&br, &size);
                ok &= gst_byte_reader_get_uint32_le (&br, &tag);
                if (!ok || size < 8) {
                  GST_WARNING_OBJECT (qtdemux,
                      "Failed to walk optional chunk list");
                  break;
                }
                GST_DEBUG_OBJECT (qtdemux,
                    "Found optional %4.4s chunk, size %u", (const char *) &tag,
                    size);
                if (tag == FOURCC_fiel) {
                  guint8 n_fields, ordering;
                  gst_byte_reader_get_uint8 (&br, &n_fields);
                  gst_byte_reader_get_uint8 (&br, &ordering);
                  if (n_fields == 1 || n_fields == 2) {
                    GST_DEBUG_OBJECT (qtdemux,
                        "Found fiel tag with %u fields, ordering %u", n_fields,
                        ordering);
                    if (n_fields == 2)
                      gst_caps_set_simple (stream->caps, "interlace-mode",
                          G_TYPE_STRING, "interleaved", NULL);
                  } else {
                    GST_WARNING_OBJECT (qtdemux,
                        "Found fiel tag with invalid fields (%u)", n_fields);
                  }
                }
                offset += size;
              }
            } else {
              GST_DEBUG_OBJECT (qtdemux,
                  "Color table ID is 0, not trying to get interlacedness");
            }
          } else {
            GST_WARNING_OBJECT (qtdemux,
                "Length of jpeg chunk is too small, not trying to get interlacedness");
          }

          break;
        }
        case FOURCC_SVQ3:
        case FOURCC_VP31:
        {
          GstBuffer *buf;
          GstBuffer *seqh = NULL;
          guint8 *gamma_data = NULL;
          gint len = QT_UINT32 (stsd_data);

          qtdemux_parse_svq3_stsd_data (qtdemux, stsd, &gamma_data, &seqh);
          if (gamma_data) {
            gst_caps_set_simple (stream->caps, "applied-gamma", G_TYPE_DOUBLE,
                QT_FP32 (gamma_data), NULL);
          }
          if (seqh) {
            /* sorry for the bad name, but we don't know what this is, other
             * than its own fourcc */
            gst_caps_set_simple (stream->caps, "seqh", GST_TYPE_BUFFER, seqh,
                NULL);
          }

          GST_DEBUG_OBJECT (qtdemux, "found codec_data in stsd");
          buf = gst_buffer_new_and_alloc (len);
          gst_buffer_fill (buf, 0, stsd_data, len);
          gst_caps_set_simple (stream->caps,
              "codec_data", GST_TYPE_BUFFER, buf, NULL);
          gst_buffer_unref (buf);
          break;
        }
        case FOURCC_rle_:
        case FOURCC_WRLE:
        {
          gst_caps_set_simple (stream->caps,
              "depth", G_TYPE_INT, QT_UINT16 (stsd_data + offset + 82), NULL);
          break;
        }
        case FOURCC_XiTh:
        {
          GNode *xith, *xdxt;

          GST_DEBUG_OBJECT (qtdemux, "found XiTh");
          xith = qtdemux_tree_get_child_by_type (stsd, FOURCC_XiTh);
          if (!xith)
            break;

          xdxt = qtdemux_tree_get_child_by_type (xith, FOURCC_XdxT);
          if (!xdxt)
            break;

          GST_DEBUG_OBJECT (qtdemux, "found XdxT node");
          /* collect the headers and store them in a stream list so that we can
           * send them out first */
          qtdemux_parse_theora_extension (qtdemux, stream, xdxt);
          break;
        }
        case FOURCC_ovc1:
        {
          GNode *ovc1;
          guint8 *ovc1_data;
          guint ovc1_len;
          GstBuffer *buf;

          GST_DEBUG_OBJECT (qtdemux, "parse ovc1 header");
          ovc1 = qtdemux_tree_get_child_by_type (stsd, FOURCC_ovc1);
          if (!ovc1)
            break;
          ovc1_data = ovc1->data;
          ovc1_len = QT_UINT32 (ovc1_data);
          if (ovc1_len <= 198) {
            GST_WARNING_OBJECT (qtdemux, "Too small ovc1 header, skipping");
            break;
          }
          buf = gst_buffer_new_and_alloc (ovc1_len - 198);
          gst_buffer_fill (buf, 0, ovc1_data + 198, ovc1_len - 198);
          gst_caps_set_simple (stream->caps,
              "codec_data", GST_TYPE_BUFFER, buf, NULL);
          gst_buffer_unref (buf);
          break;
        }
        case FOURCC_vc_1:
        {
          gint len = QT_UINT32 (stsd_data) - 0x66;
          const guint8 *vc1_data = stsd_data + 0x66;

          /* find dvc1 */
          while (len >= 8) {
            gint size;

            if (QT_UINT32 (vc1_data) <= len)
              size = QT_UINT32 (vc1_data) - 8;
            else
              size = len - 8;

            if (size < 1)
              /* No real data, so break out */
              break;

            switch (QT_FOURCC (vc1_data + 0x4)) {
              case GST_MAKE_FOURCC ('d', 'v', 'c', '1'):
              {
                GstBuffer *buf;

                GST_DEBUG_OBJECT (qtdemux, "found dvc1 codec_data in stsd");
                buf = gst_buffer_new_and_alloc (size);
                gst_buffer_fill (buf, 0, vc1_data + 8, size);
                gst_caps_set_simple (stream->caps,
                    "codec_data", GST_TYPE_BUFFER, buf, NULL);
                gst_buffer_unref (buf);
                break;
              }
              default:
                break;
            }
            len -= size + 8;
            vc1_data += size + 8;
          }
          break;
        }
        default:
          break;
      }
    }

    GST_INFO_OBJECT (qtdemux,
        "type %" GST_FOURCC_FORMAT " caps %" GST_PTR_FORMAT,
        GST_FOURCC_ARGS (fourcc), stream->caps);

  } else if (stream->subtype == FOURCC_soun) {
    int version, samplesize;
    guint16 compression_id;
    gboolean amrwb = FALSE;

    offset = 32;
    /* sample description entry (16) + sound sample description v0 (20) */
    if (len < 36)
      goto corrupt_file;

    version = QT_UINT32 (stsd_data + offset);
    stream->n_channels = QT_UINT16 (stsd_data + offset + 8);
    samplesize = QT_UINT16 (stsd_data + offset + 10);
    compression_id = QT_UINT16 (stsd_data + offset + 12);
    stream->rate = QT_FP32 (stsd_data + offset + 16);

    GST_LOG_OBJECT (qtdemux, "version/rev:      %08x", version);
    GST_LOG_OBJECT (qtdemux, "vendor:           %08x",
        QT_UINT32 (stsd_data + offset + 4));
    GST_LOG_OBJECT (qtdemux, "n_channels:       %d", stream->n_channels);
    GST_LOG_OBJECT (qtdemux, "sample_size:      %d", samplesize);
    GST_LOG_OBJECT (qtdemux, "compression_id:   %d", compression_id);
    GST_LOG_OBJECT (qtdemux, "packet size:      %d",
        QT_UINT16 (stsd_data + offset + 14));
    GST_LOG_OBJECT (qtdemux, "sample rate:      %g", stream->rate);

    if (compression_id == 0xfffe)
      stream->sampled = TRUE;

    /* first assume uncompressed audio */
    stream->bytes_per_sample = samplesize / 8;
    stream->samples_per_frame = stream->n_channels;
    stream->bytes_per_frame = stream->n_channels * stream->bytes_per_sample;
    stream->samples_per_packet = stream->samples_per_frame;
    stream->bytes_per_packet = stream->bytes_per_sample;

    offset = 52;
    switch (fourcc) {
        /* Yes, these have to be hard-coded */
      case FOURCC_MAC6:
      {
        stream->samples_per_packet = 6;
        stream->bytes_per_packet = 1;
        stream->bytes_per_frame = 1 * stream->n_channels;
        stream->bytes_per_sample = 1;
        stream->samples_per_frame = 6 * stream->n_channels;
        break;
      }
      case FOURCC_MAC3:
      {
        stream->samples_per_packet = 3;
        stream->bytes_per_packet = 1;
        stream->bytes_per_frame = 1 * stream->n_channels;
        stream->bytes_per_sample = 1;
        stream->samples_per_frame = 3 * stream->n_channels;
        break;
      }
      case FOURCC_ima4:
      {
        stream->samples_per_packet = 64;
        stream->bytes_per_packet = 34;
        stream->bytes_per_frame = 34 * stream->n_channels;
        stream->bytes_per_sample = 2;
        stream->samples_per_frame = 64 * stream->n_channels;
        break;
      }
      case FOURCC_ulaw:
      case FOURCC_alaw:
      {
        stream->samples_per_packet = 1;
        stream->bytes_per_packet = 1;
        stream->bytes_per_frame = 1 * stream->n_channels;
        stream->bytes_per_sample = 1;
        stream->samples_per_frame = 1 * stream->n_channels;
        break;
      }
      case FOURCC_agsm:
      {
        stream->samples_per_packet = 160;
        stream->bytes_per_packet = 33;
        stream->bytes_per_frame = 33 * stream->n_channels;
        stream->bytes_per_sample = 2;
        stream->samples_per_frame = 160 * stream->n_channels;
        break;
      }
      default:
        break;
    }

    if (version == 0x00010000) {
      /* sample description entry (16) + sound sample description v1 (20+16) */
      if (len < 52)
        goto corrupt_file;

      switch (fourcc) {
        case FOURCC_twos:
        case FOURCC_sowt:
        case FOURCC_raw_:
          break;
        default:
        {
          /* only parse extra decoding config for non-pcm audio */
          stream->samples_per_packet = QT_UINT32 (stsd_data + offset);
          stream->bytes_per_packet = QT_UINT32 (stsd_data + offset + 4);
          stream->bytes_per_frame = QT_UINT32 (stsd_data + offset + 8);
          stream->bytes_per_sample = QT_UINT32 (stsd_data + offset + 12);

          GST_LOG_OBJECT (qtdemux, "samples/packet:   %d",
              stream->samples_per_packet);
          GST_LOG_OBJECT (qtdemux, "bytes/packet:     %d",
              stream->bytes_per_packet);
          GST_LOG_OBJECT (qtdemux, "bytes/frame:      %d",
              stream->bytes_per_frame);
          GST_LOG_OBJECT (qtdemux, "bytes/sample:     %d",
              stream->bytes_per_sample);

          if (!stream->sampled && stream->bytes_per_packet) {
            stream->samples_per_frame = (stream->bytes_per_frame /
                stream->bytes_per_packet) * stream->samples_per_packet;
            GST_LOG_OBJECT (qtdemux, "samples/frame:    %d",
                stream->samples_per_frame);
          }
          break;
        }
      }
    } else if (version == 0x00020000) {
      union
      {
        gdouble fp;
        guint64 val;
      } qtfp;

      /* sample description entry (16) + sound sample description v2 (56) */
      if (len < 72)
        goto corrupt_file;

      qtfp.val = QT_UINT64 (stsd_data + offset + 4);
      stream->rate = qtfp.fp;
      stream->n_channels = QT_UINT32 (stsd_data + offset + 12);

      GST_LOG_OBJECT (qtdemux, "Sound sample description Version 2");
      GST_LOG_OBJECT (qtdemux, "sample rate:        %g", stream->rate);
      GST_LOG_OBJECT (qtdemux, "n_channels:         %d", stream->n_channels);
      GST_LOG_OBJECT (qtdemux, "bits/channel:       %d",
          QT_UINT32 (stsd_data + offset + 20));
      GST_LOG_OBJECT (qtdemux, "format flags:       %X",
          QT_UINT32 (stsd_data + offset + 24));
      GST_LOG_OBJECT (qtdemux, "bytes/packet:       %d",
          QT_UINT32 (stsd_data + offset + 28));
      GST_LOG_OBJECT (qtdemux, "LPCM frames/packet: %d",
          QT_UINT32 (stsd_data + offset + 32));
    } else if (version != 0x00000) {
      GST_WARNING_OBJECT (qtdemux, "unknown audio STSD version %08x", version);
    }

    if (stream->caps)
      gst_caps_unref (stream->caps);

    stream->caps = qtdemux_audio_caps (qtdemux, stream, fourcc,
        stsd_data + 32, len - 16, &codec);

    switch (fourcc) {
      case FOURCC_in24:
      {
        GNode *enda;
        GNode *in24;

        in24 = qtdemux_tree_get_child_by_type (stsd, FOURCC_in24);

        enda = qtdemux_tree_get_child_by_type (in24, FOURCC_enda);
        if (!enda) {
          wave = qtdemux_tree_get_child_by_type (in24, FOURCC_wave);
          if (wave)
            enda = qtdemux_tree_get_child_by_type (wave, FOURCC_enda);
        }
        if (enda) {
          int enda_value = QT_UINT16 ((guint8 *) enda->data + 8);
          gst_caps_set_simple (stream->caps,
              "format", G_TYPE_STRING, (enda_value) ? "S24LE" : "S24BE", NULL);
        }
        break;
      }
      case FOURCC_owma:
      {
        GNode *owma;
        const guint8 *owma_data;
        const gchar *codec_name = NULL;
        guint owma_len;
        GstBuffer *buf;
        gint version = 1;
        /* from http://msdn.microsoft.com/en-us/library/dd757720(VS.85).aspx */
        /* FIXME this should also be gst_riff_strf_auds,
         * but the latter one is actually missing bits-per-sample :( */
        typedef struct
        {
          gint16 wFormatTag;
          gint16 nChannels;
          gint32 nSamplesPerSec;
          gint32 nAvgBytesPerSec;
          gint16 nBlockAlign;
          gint16 wBitsPerSample;
          gint16 cbSize;
        } WAVEFORMATEX;
        WAVEFORMATEX *wfex;

        GST_DEBUG_OBJECT (qtdemux, "parse owma");
        owma = qtdemux_tree_get_child_by_type (stsd, FOURCC_owma);
        if (!owma)
          break;
        owma_data = owma->data;
        owma_len = QT_UINT32 (owma_data);
        if (owma_len <= 54) {
          GST_WARNING_OBJECT (qtdemux, "Too small owma header, skipping");
          break;
        }
        wfex = (WAVEFORMATEX *) (owma_data + 36);
        buf = gst_buffer_new_and_alloc (owma_len - 54);
        gst_buffer_fill (buf, 0, owma_data + 54, owma_len - 54);
        if (wfex->wFormatTag == 0x0161) {
          codec_name = "Windows Media Audio";
          version = 2;
        } else if (wfex->wFormatTag == 0x0162) {
          codec_name = "Windows Media Audio 9 Pro";
          version = 3;
        } else if (wfex->wFormatTag == 0x0163) {
          codec_name = "Windows Media Audio 9 Lossless";
          /* is that correct? gstffmpegcodecmap.c is missing it, but
           * fluendo codec seems to support it */
          version = 4;
        }

        gst_caps_set_simple (stream->caps,
            "codec_data", GST_TYPE_BUFFER, buf,
            "wmaversion", G_TYPE_INT, version,
            "block_align", G_TYPE_INT, GST_READ_UINT16_LE (&wfex->nBlockAlign),
            "bitrate", G_TYPE_INT, GST_READ_UINT32_LE (&wfex->nAvgBytesPerSec),
            "width", G_TYPE_INT, GST_READ_UINT16_LE (&wfex->wBitsPerSample),
            "depth", G_TYPE_INT, GST_READ_UINT16_LE (&wfex->wBitsPerSample),
            NULL);
        gst_buffer_unref (buf);

        if (codec_name) {
          g_free (codec);
          codec = g_strdup (codec_name);
        }
        break;
      }
      case FOURCC_wma_:
      {
        gint len = QT_UINT32 (stsd_data) - offset;
        const guint8 *wfex_data = stsd_data + offset;
        const gchar *codec_name = NULL;
        gint version = 1;
        /* from http://msdn.microsoft.com/en-us/library/dd757720(VS.85).aspx */
        /* FIXME this should also be gst_riff_strf_auds,
         * but the latter one is actually missing bits-per-sample :( */
        typedef struct
        {
          gint16 wFormatTag;
          gint16 nChannels;
          gint32 nSamplesPerSec;
          gint32 nAvgBytesPerSec;
          gint16 nBlockAlign;
          gint16 wBitsPerSample;
          gint16 cbSize;
        } WAVEFORMATEX;
        WAVEFORMATEX wfex;

        /* FIXME: unify with similar wavformatex parsing code above */
        GST_DEBUG_OBJECT (qtdemux, "parse wma, looking for wfex");

        /* find wfex */
        while (len >= 8) {
          gint size;

          if (QT_UINT32 (wfex_data) <= len)
            size = QT_UINT32 (wfex_data) - 8;
          else
            size = len - 8;

          if (size < 1)
            /* No real data, so break out */
            break;

          switch (QT_FOURCC (wfex_data + 4)) {
            case GST_MAKE_FOURCC ('w', 'f', 'e', 'x'):
            {
              GST_DEBUG_OBJECT (qtdemux, "found wfex in stsd");

              if (size < 8 + 18)
                break;

              wfex.wFormatTag = GST_READ_UINT16_LE (wfex_data + 8 + 0);
              wfex.nChannels = GST_READ_UINT16_LE (wfex_data + 8 + 2);
              wfex.nSamplesPerSec = GST_READ_UINT32_LE (wfex_data + 8 + 4);
              wfex.nAvgBytesPerSec = GST_READ_UINT32_LE (wfex_data + 8 + 8);
              wfex.nBlockAlign = GST_READ_UINT16_LE (wfex_data + 8 + 12);
              wfex.wBitsPerSample = GST_READ_UINT16_LE (wfex_data + 8 + 14);
              wfex.cbSize = GST_READ_UINT16_LE (wfex_data + 8 + 16);

              GST_LOG_OBJECT (qtdemux, "Found wfex box in stsd:");
              GST_LOG_OBJECT (qtdemux, "FormatTag = 0x%04x, Channels = %u, "
                  "SamplesPerSec = %u, AvgBytesPerSec = %u, BlockAlign = %u, "
                  "BitsPerSample = %u, Size = %u", wfex.wFormatTag,
                  wfex.nChannels, wfex.nSamplesPerSec, wfex.nAvgBytesPerSec,
                  wfex.nBlockAlign, wfex.wBitsPerSample, wfex.cbSize);

              if (wfex.wFormatTag == 0x0161) {
                codec_name = "Windows Media Audio";
                version = 2;
              } else if (wfex.wFormatTag == 0x0162) {
                codec_name = "Windows Media Audio 9 Pro";
                version = 3;
              } else if (wfex.wFormatTag == 0x0163) {
                codec_name = "Windows Media Audio 9 Lossless";
                /* is that correct? gstffmpegcodecmap.c is missing it, but
                 * fluendo codec seems to support it */
                version = 4;
              }

              gst_caps_set_simple (stream->caps,
                  "wmaversion", G_TYPE_INT, version,
                  "block_align", G_TYPE_INT, wfex.nBlockAlign,
                  "bitrate", G_TYPE_INT, wfex.nAvgBytesPerSec,
                  "width", G_TYPE_INT, wfex.wBitsPerSample,
                  "depth", G_TYPE_INT, wfex.wBitsPerSample, NULL);

              if (size > wfex.cbSize) {
                GstBuffer *buf;

                buf = gst_buffer_new_and_alloc (size - wfex.cbSize);
                gst_buffer_fill (buf, 0, wfex_data + 8 + wfex.cbSize,
                    size - wfex.cbSize);
                gst_caps_set_simple (stream->caps,
                    "codec_data", GST_TYPE_BUFFER, buf, NULL);
                gst_buffer_unref (buf);
              } else {
                GST_WARNING_OBJECT (qtdemux, "no codec data");
              }

              if (codec_name) {
                g_free (codec);
                codec = g_strdup (codec_name);
              }
              break;
            }
            default:
              break;
          }
          len -= size + 8;
          wfex_data += size + 8;
        }
        break;
      }
      case FOURCC_opus:
      {
        GNode *opus;
        const guint8 *opus_data;
        guint8 *channel_mapping = NULL;
        guint32 rate;
        guint8 channels;
        guint8 channel_mapping_family;
        guint8 stream_count;
        guint8 coupled_count;
        guint8 i;

        opus = qtdemux_tree_get_child_by_type (stsd, FOURCC_opus);
        opus_data = opus->data;

        channels = GST_READ_UINT8 (opus_data + 45);
        rate = GST_READ_UINT32_LE (opus_data + 48);
        channel_mapping_family = GST_READ_UINT8 (opus_data + 54);
        stream_count = GST_READ_UINT8 (opus_data + 55);
        coupled_count = GST_READ_UINT8 (opus_data + 56);

        if (channels > 0) {
          channel_mapping = g_malloc (channels * sizeof (guint8));
          for (i = 0; i < channels; i++)
            channel_mapping[i] = GST_READ_UINT8 (opus_data + i + 57);
        }

        stream->caps = gst_codec_utils_opus_create_caps (rate, channels,
            channel_mapping_family, stream_count, coupled_count,
            channel_mapping);
        break;
      }
      default:
        break;
    }

    if (codec) {
      GstStructure *s;
      gint bitrate = 0;

      gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
          GST_TAG_AUDIO_CODEC, codec, NULL);
      g_free (codec);
      codec = NULL;

      /* some bitrate info may have ended up in caps */
      s = gst_caps_get_structure (stream->caps, 0);
      gst_structure_get_int (s, "bitrate", &bitrate);
      if (bitrate > 0)
        gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
            GST_TAG_BITRATE, bitrate, NULL);
    }

    if (stream->protected && fourcc == FOURCC_mp4a)
      mp4a = qtdemux_tree_get_child_by_type (stsd, FOURCC_enca);
    else
      mp4a = qtdemux_tree_get_child_by_type (stsd, FOURCC_mp4a);

    wave = NULL;
    esds = NULL;
    if (mp4a) {
      wave = qtdemux_tree_get_child_by_type (mp4a, FOURCC_wave);
      if (wave)
        esds = qtdemux_tree_get_child_by_type (wave, FOURCC_esds);
      if (!esds)
        esds = qtdemux_tree_get_child_by_type (mp4a, FOURCC_esds);
    }


    /* If the fourcc's bottom 16 bits gives 'sm', then the top
       16 bits is a byte-swapped wave-style codec identifier,
       and we can find a WAVE header internally to a 'wave' atom here.
       This can more clearly be thought of as 'ms' as the top 16 bits, and a
       codec id as the bottom 16 bits - but byte-swapped to store in QT (which
       is big-endian).
     */
    if ((fourcc & 0xffff) == (('s' << 8) | 'm')) {
      if (len < offset + 20) {
        GST_WARNING_OBJECT (qtdemux, "No wave atom in MS-style audio");
      } else {
        guint32 datalen = QT_UINT32 (stsd_data + offset + 16);
        const guint8 *data = stsd_data + offset + 16;
        GNode *wavenode;
        GNode *waveheadernode;

        wavenode = g_node_new ((guint8 *) data);
        if (qtdemux_parse_node (qtdemux, wavenode, data, datalen)) {
          const guint8 *waveheader;
          guint32 headerlen;

          waveheadernode = qtdemux_tree_get_child_by_type (wavenode, fourcc);
          if (waveheadernode) {
            waveheader = (const guint8 *) waveheadernode->data;
            headerlen = QT_UINT32 (waveheader);

            if (headerlen > 8) {
              gst_riff_strf_auds *header = NULL;
              GstBuffer *headerbuf;
              GstBuffer *extra;

              waveheader += 8;
              headerlen -= 8;

              headerbuf = gst_buffer_new_and_alloc (headerlen);
              gst_buffer_fill (headerbuf, 0, waveheader, headerlen);

              if (gst_riff_parse_strf_auds (GST_ELEMENT_CAST (qtdemux),
                      headerbuf, &header, &extra)) {
                gst_caps_unref (stream->caps);
                /* FIXME: Need to do something with the channel reorder map */
                stream->caps = gst_riff_create_audio_caps (header->format, NULL,
                    header, extra, NULL, NULL, NULL);

                if (extra)
                  gst_buffer_unref (extra);
                g_free (header);
              }
            }
          } else
            GST_DEBUG ("Didn't find waveheadernode for this codec");
        }
        g_node_destroy (wavenode);
      }
    } else if (esds) {
      gst_qtdemux_handle_esds (qtdemux, stream, esds, stream->pending_tags);
    } else {
      switch (fourcc) {
#if 0
          /* FIXME: what is in the chunk? */
        case FOURCC_QDMC:
        {
          gint len = QT_UINT32 (stsd_data);

          /* seems to be always = 116 = 0x74 */
          break;
        }
#endif
        case FOURCC_QDM2:
        {
          gint len = QT_UINT32 (stsd_data);

          if (len > 0x4C) {
            GstBuffer *buf = gst_buffer_new_and_alloc (len - 0x4C);

            gst_buffer_fill (buf, 0, stsd_data + 0x4C, len - 0x4C);
            gst_caps_set_simple (stream->caps,
                "codec_data", GST_TYPE_BUFFER, buf, NULL);
            gst_buffer_unref (buf);
          }
          gst_caps_set_simple (stream->caps,
              "samplesize", G_TYPE_INT, samplesize, NULL);
          break;
        }
        case FOURCC_alac:
        {
          GNode *alac, *wave = NULL;

          /* apparently, m4a has this atom appended directly in the stsd entry,
           * while mov has it in a wave atom */
          alac = qtdemux_tree_get_child_by_type (stsd, FOURCC_alac);
          if (alac) {
            /* alac now refers to stsd entry atom */
            wave = qtdemux_tree_get_child_by_type (alac, FOURCC_wave);
            if (wave)
              alac = qtdemux_tree_get_child_by_type (wave, FOURCC_alac);
            else
              alac = qtdemux_tree_get_child_by_type (alac, FOURCC_alac);
          }
          if (alac) {
            const guint8 *alac_data = alac->data;
            gint len = QT_UINT32 (alac->data);
            GstBuffer *buf;

            if (len < 36) {
              GST_DEBUG_OBJECT (qtdemux,
                  "discarding alac atom with unexpected len %d", len);
            } else {
              /* codec-data contains alac atom size and prefix,
               * ffmpeg likes it that way, not quite gst-ish though ...*/
              buf = gst_buffer_new_and_alloc (len);
              gst_buffer_fill (buf, 0, alac->data, len);
              gst_caps_set_simple (stream->caps,
                  "codec_data", GST_TYPE_BUFFER, buf, NULL);
              gst_buffer_unref (buf);

              stream->bytes_per_frame = QT_UINT32 (alac_data + 12);
              stream->n_channels = QT_UINT8 (alac_data + 21);
              stream->rate = QT_UINT32 (alac_data + 32);
            }
          }
          gst_caps_set_simple (stream->caps,
              "samplesize", G_TYPE_INT, samplesize, NULL);
          break;
        }
        case FOURCC_fLaC:
        {
          /* The codingname of the sample entry is 'fLaC' */
          GNode *flac = qtdemux_tree_get_child_by_type (stsd, FOURCC_fLaC);

          if (flac) {
            /* The 'dfLa' box is added to the sample entry to convey
               initializing information for the decoder. */
            const GNode *dfla =
                qtdemux_tree_get_child_by_type (flac, FOURCC_dfLa);

            if (dfla) {
              const guint32 len = QT_UINT32 (dfla->data);

              /* Must contain at least dfLa box header (12),
               * METADATA_BLOCK_HEADER (4), METADATA_BLOCK_STREAMINFO (34) */
              if (len < 50) {
                GST_DEBUG_OBJECT (qtdemux,
                    "discarding dfla atom with unexpected len %d", len);
              } else {
                /* skip dfLa header to get the METADATA_BLOCKs */
                const guint8 *metadata_blocks = (guint8 *) dfla->data + 12;
                const guint32 metadata_blocks_len = len - 12;

                gchar *stream_marker = g_strdup ("fLaC");
                GstBuffer *block = gst_buffer_new_wrapped (stream_marker,
                    strlen (stream_marker));

                guint32 index = 0;
                guint32 remainder = 0;
                guint32 block_size = 0;
                gboolean is_last = FALSE;

                GValue array = G_VALUE_INIT;
                GValue value = G_VALUE_INIT;

                g_value_init (&array, GST_TYPE_ARRAY);
                g_value_init (&value, GST_TYPE_BUFFER);

                gst_value_set_buffer (&value, block);
                gst_value_array_append_value (&array, &value);
                g_value_reset (&value);

                gst_buffer_unref (block);

                /* check there's at least one METADATA_BLOCK_HEADER's worth
                 * of data, and we haven't already finished parsing */
                while (!is_last && ((index + 3) < metadata_blocks_len)) {
                  remainder = metadata_blocks_len - index;

                  /* add the METADATA_BLOCK_HEADER size to the signalled size */
                  block_size = 4 +
                      (metadata_blocks[index + 1] << 16) +
                      (metadata_blocks[index + 2] << 8) +
                      metadata_blocks[index + 3];

                  /* be careful not to read off end of box */
                  if (block_size > remainder) {
                    break;
                  }

                  is_last = metadata_blocks[index] >> 7;

                  block = gst_buffer_new_and_alloc (block_size);

                  gst_buffer_fill (block, 0, &metadata_blocks[index],
                      block_size);

                  gst_value_set_buffer (&value, block);
                  gst_value_array_append_value (&array, &value);
                  g_value_reset (&value);

                  gst_buffer_unref (block);

                  index += block_size;
                }

                /* only append the metadata if we successfully read all of it */
                if (is_last) {
                  gst_structure_set_value (gst_caps_get_structure (stream->caps,
                          0), "streamheader", &array);
                } else {
                  GST_WARNING_OBJECT (qtdemux,
                      "discarding all METADATA_BLOCKs due to invalid "
                      "block_size %d at idx %d, rem %d", block_size, index,
                      remainder);
                }

                g_value_unset (&value);
                g_value_unset (&array);

                /* The sample rate obtained from the stsd may not be accurate
                 * since it cannot represent rates greater than 65535Hz, so
                 * override that value with the sample rate from the
                 * METADATA_BLOCK_STREAMINFO block */
                stream->rate =
                    (QT_UINT32 (metadata_blocks + 14) >> 12) & 0xFFFFF;
              }
            }
          }
          break;
        }
        case FOURCC_sawb:
          /* Fallthrough! */
          amrwb = TRUE;
        case FOURCC_samr:
        {
          gint len = QT_UINT32 (stsd_data);

          if (len > 0x34) {
            GstBuffer *buf = gst_buffer_new_and_alloc (len - 0x34);
            guint bitrate;

            gst_buffer_fill (buf, 0, stsd_data + 0x34, len - 0x34);

            /* If we have enough data, let's try to get the 'damr' atom. See
             * the 3GPP container spec (26.244) for more details. */
            if ((len - 0x34) > 8 &&
                (bitrate = qtdemux_parse_amr_bitrate (buf, amrwb))) {
              gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
                  GST_TAG_MAXIMUM_BITRATE, bitrate, NULL);
            }

            gst_caps_set_simple (stream->caps,
                "codec_data", GST_TYPE_BUFFER, buf, NULL);
            gst_buffer_unref (buf);
          }
          break;
        }
        case FOURCC_mp4a:
        {
          /* mp4a atom withtout ESDS; Attempt to build codec data from atom */
          gint len = QT_UINT32 (stsd_data);

          if (len >= 50) {
            guint16 sound_version = QT_UINT16 (stsd_data + 32);

            if (sound_version == 1) {
              guint16 channels = QT_UINT16 (stsd_data + 40);
              guint32 time_scale = QT_UINT32 (stsd_data + 46);
              guint8 codec_data[2];
              GstBuffer *buf;
              gint profile = 2; /* FIXME: Can this be determined somehow? There doesn't seem to be anything in mp4a atom that specifis compression */

              gint sample_rate_index =
                  gst_codec_utils_aac_get_index_from_sample_rate (time_scale);

              /* build AAC codec data */
              codec_data[0] = profile << 3;
              codec_data[0] |= ((sample_rate_index >> 1) & 0x7);
              codec_data[1] = (sample_rate_index & 0x01) << 7;
              codec_data[1] |= (channels & 0xF) << 3;

              buf = gst_buffer_new_and_alloc (2);
              gst_buffer_fill (buf, 0, codec_data, 2);
              gst_caps_set_simple (stream->caps,
                  "codec_data", GST_TYPE_BUFFER, buf, NULL);
              gst_buffer_unref (buf);
            }
          }
          break;
        }
        default:
          GST_INFO_OBJECT (qtdemux,
              "unhandled type %" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
          break;
      }
    }
    GST_INFO_OBJECT (qtdemux,
        "type %" GST_FOURCC_FORMAT " caps %" GST_PTR_FORMAT,
        GST_FOURCC_ARGS (fourcc), stream->caps);

  } else if (stream->subtype == FOURCC_strm) {
    if (fourcc == FOURCC_rtsp) {
      stream->redirect_uri = qtdemux_get_rtsp_uri_from_hndl (qtdemux, minf);
    } else {
      GST_INFO_OBJECT (qtdemux, "unhandled stream type %" GST_FOURCC_FORMAT,
          GST_FOURCC_ARGS (fourcc));
      goto unknown_stream;
    }
    stream->sampled = TRUE;
  } else if (stream->subtype == FOURCC_subp || stream->subtype == FOURCC_text
      || stream->subtype == FOURCC_sbtl || stream->subtype == FOURCC_subt) {

    stream->sampled = TRUE;
    stream->sparse = TRUE;

    stream->caps =
        qtdemux_sub_caps (qtdemux, stream, fourcc, stsd_data, &codec);
    if (codec) {
      gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
          GST_TAG_SUBTITLE_CODEC, codec, NULL);
      g_free (codec);
      codec = NULL;
    }

    /* hunt for sort-of codec data */
    switch (fourcc) {
      case FOURCC_mp4s:
      {
        GNode *mp4s = NULL;
        GNode *esds = NULL;

        /* look for palette in a stsd->mp4s->esds sub-atom */
        mp4s = qtdemux_tree_get_child_by_type (stsd, FOURCC_mp4s);
        if (mp4s)
          esds = qtdemux_tree_get_child_by_type (mp4s, FOURCC_esds);
        if (esds == NULL) {
          /* Invalid STSD */
          GST_LOG_OBJECT (qtdemux, "Skipping invalid stsd: no esds child");
          break;
        }

        gst_qtdemux_handle_esds (qtdemux, stream, esds, stream->pending_tags);
        break;
      }
      default:
        GST_INFO_OBJECT (qtdemux,
            "unhandled type %" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
        break;
    }
    GST_INFO_OBJECT (qtdemux,
        "type %" GST_FOURCC_FORMAT " caps %" GST_PTR_FORMAT,
        GST_FOURCC_ARGS (fourcc), stream->caps);
  } else {
    /* everything in 1 sample */
    stream->sampled = TRUE;

    stream->caps =
        qtdemux_generic_caps (qtdemux, stream, fourcc, stsd_data, &codec);

    if (stream->caps == NULL)
      goto unknown_stream;

    if (codec) {
      gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
          GST_TAG_SUBTITLE_CODEC, codec, NULL);
      g_free (codec);
      codec = NULL;
    }
  }

  /* promote to sampled format */
  if (stream->fourcc == FOURCC_samr) {
    /* force mono 8000 Hz for AMR */
    stream->sampled = TRUE;
    stream->n_channels = 1;
    stream->rate = 8000;
  } else if (stream->fourcc == FOURCC_sawb) {
    /* force mono 16000 Hz for AMR-WB */
    stream->sampled = TRUE;
    stream->n_channels = 1;
    stream->rate = 16000;
  } else if (stream->fourcc == FOURCC_mp4a) {
    stream->sampled = TRUE;
  }

  /* collect sample information */
  if (!qtdemux_stbl_init (qtdemux, stream, stbl))
    goto samples_failed;

  if (qtdemux->fragmented) {
    guint64 offset;

    /* need all moov samples as basis; probably not many if any at all */
    /* prevent moof parsing taking of at this time */
    offset = qtdemux->moof_offset;
    qtdemux->moof_offset = 0;
    if (stream->n_samples &&
        !qtdemux_parse_samples (qtdemux, stream, stream->n_samples - 1)) {
      qtdemux->moof_offset = offset;
      goto samples_failed;
    }
    qtdemux->moof_offset = 0;
    /* movie duration more reliable in this case (e.g. mehd) */
    if (qtdemux->segment.duration &&
        GST_CLOCK_TIME_IS_VALID (qtdemux->segment.duration))
      stream->duration =
          GSTTIME_TO_QTSTREAMTIME (stream, qtdemux->segment.duration);
  }

  /* configure segments */
  if (!qtdemux_parse_segments (qtdemux, stream, trak))
    goto segments_failed;

  /* add some language tag, if useful */
  if (stream->lang_id[0] != '\0' && strcmp (stream->lang_id, "unk") &&
      strcmp (stream->lang_id, "und")) {
    const gchar *lang_code;

    /* convert ISO 639-2 code to ISO 639-1 */
    lang_code = gst_tag_get_language_code (stream->lang_id);
    gst_tag_list_add (stream->pending_tags, GST_TAG_MERGE_REPLACE,
        GST_TAG_LANGUAGE_CODE, (lang_code) ? lang_code : stream->lang_id, NULL);
  }

  /* Check for UDTA tags */
  if ((udta = qtdemux_tree_get_child_by_type (trak, FOURCC_udta))) {
    qtdemux_parse_udta (qtdemux, stream->pending_tags, udta);
  }

  /* now we are ready to add the stream */
  if (qtdemux->n_streams >= GST_QTDEMUX_MAX_STREAMS)
    goto too_many_streams;

  if (!qtdemux->got_moov) {
    qtdemux->streams[qtdemux->n_streams] = stream;
    qtdemux->n_streams++;
    GST_DEBUG_OBJECT (qtdemux, "n_streams is now %d", qtdemux->n_streams);
  }

  return TRUE;

/* ERRORS */
skip_track:
  {
    GST_INFO_OBJECT (qtdemux, "skip disabled track");
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return TRUE;
  }
corrupt_file:
  {
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file is corrupt and cannot be played.")), (NULL));
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return FALSE;
  }
error_encrypted:
  {
    GST_ELEMENT_ERROR (qtdemux, STREAM, DECRYPT, (NULL), (NULL));
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return FALSE;
  }
samples_failed:
segments_failed:
  {
    /* we posted an error already */
    /* free stbl sub-atoms */
    gst_qtdemux_stbl_free (stream);
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return FALSE;
  }
existing_stream:
  {
    GST_INFO_OBJECT (qtdemux, "stream with track id %i already exists",
        track_id);
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return TRUE;
  }
unknown_stream:
  {
    GST_INFO_OBJECT (qtdemux, "unknown subtype %" GST_FOURCC_FORMAT,
        GST_FOURCC_ARGS (stream->subtype));
    if (new_stream)
      gst_qtdemux_stream_free (qtdemux, stream);
    return TRUE;
  }
too_many_streams:
  {
    GST_ELEMENT_WARNING (qtdemux, STREAM, DEMUX,
        (_("This file contains too many streams. Only playing first %d"),
            GST_QTDEMUX_MAX_STREAMS), (NULL));
    return TRUE;
  }
}