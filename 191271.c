gst_qtdemux_handle_esds (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GNode * esds, GstTagList * list)
{
  int len = QT_UINT32 (esds->data);
  guint8 *ptr = esds->data;
  guint8 *end = ptr + len;
  int tag;
  guint8 *data_ptr = NULL;
  int data_len = 0;
  guint8 object_type_id = 0;
  const char *codec_name = NULL;
  GstCaps *caps = NULL;

  GST_MEMDUMP_OBJECT (qtdemux, "esds", ptr, len);
  ptr += 8;
  GST_DEBUG_OBJECT (qtdemux, "version/flags = %08x", QT_UINT32 (ptr));
  ptr += 4;
  while (ptr + 1 < end) {
    tag = QT_UINT8 (ptr);
    GST_DEBUG_OBJECT (qtdemux, "tag = %02x", tag);
    ptr++;
    len = read_descr_size (ptr, end, &ptr);
    GST_DEBUG_OBJECT (qtdemux, "len = %d", len);

    /* Check the stated amount of data is available for reading */
    if (len < 0 || ptr + len > end)
      break;

    switch (tag) {
      case ES_DESCRIPTOR_TAG:
        GST_DEBUG_OBJECT (qtdemux, "ID %04x", QT_UINT16 (ptr));
        GST_DEBUG_OBJECT (qtdemux, "priority %04x", QT_UINT8 (ptr + 2));
        ptr += 3;
        break;
      case DECODER_CONFIG_DESC_TAG:{
        guint max_bitrate, avg_bitrate;

        object_type_id = QT_UINT8 (ptr);
        max_bitrate = QT_UINT32 (ptr + 5);
        avg_bitrate = QT_UINT32 (ptr + 9);
        GST_DEBUG_OBJECT (qtdemux, "object_type_id %02x", object_type_id);
        GST_DEBUG_OBJECT (qtdemux, "stream_type %02x", QT_UINT8 (ptr + 1));
        GST_DEBUG_OBJECT (qtdemux, "buffer_size_db %02x", QT_UINT24 (ptr + 2));
        GST_DEBUG_OBJECT (qtdemux, "max bitrate %u", max_bitrate);
        GST_DEBUG_OBJECT (qtdemux, "avg bitrate %u", avg_bitrate);
        if (max_bitrate > 0 && max_bitrate < G_MAXUINT32) {
          gst_tag_list_add (list, GST_TAG_MERGE_REPLACE,
              GST_TAG_MAXIMUM_BITRATE, max_bitrate, NULL);
        }
        if (avg_bitrate > 0 && avg_bitrate < G_MAXUINT32) {
          gst_tag_list_add (list, GST_TAG_MERGE_REPLACE, GST_TAG_BITRATE,
              avg_bitrate, NULL);
        }
        ptr += 13;
        break;
      }
      case DECODER_SPECIFIC_INFO_TAG:
        GST_MEMDUMP_OBJECT (qtdemux, "data", ptr, len);
        if (object_type_id == 0xe0 && len == 0x40) {
          guint8 *data;
          GstStructure *s;
          guint32 clut[16];
          gint i;

          GST_DEBUG_OBJECT (qtdemux,
              "Have VOBSUB palette. Creating palette event");
          /* move to decConfigDescr data and read palette */
          data = ptr;
          for (i = 0; i < 16; i++) {
            clut[i] = QT_UINT32 (data);
            data += 4;
          }

          s = gst_structure_new ("application/x-gst-dvd", "event",
              G_TYPE_STRING, "dvd-spu-clut-change",
              "clut00", G_TYPE_INT, clut[0], "clut01", G_TYPE_INT, clut[1],
              "clut02", G_TYPE_INT, clut[2], "clut03", G_TYPE_INT, clut[3],
              "clut04", G_TYPE_INT, clut[4], "clut05", G_TYPE_INT, clut[5],
              "clut06", G_TYPE_INT, clut[6], "clut07", G_TYPE_INT, clut[7],
              "clut08", G_TYPE_INT, clut[8], "clut09", G_TYPE_INT, clut[9],
              "clut10", G_TYPE_INT, clut[10], "clut11", G_TYPE_INT, clut[11],
              "clut12", G_TYPE_INT, clut[12], "clut13", G_TYPE_INT, clut[13],
              "clut14", G_TYPE_INT, clut[14], "clut15", G_TYPE_INT, clut[15],
              NULL);

          /* store event and trigger custom processing */
          stream->pending_event =
              gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, s);
        } else {
          /* Generic codec_data handler puts it on the caps */
          data_ptr = ptr;
          data_len = len;
        }

        ptr += len;
        break;
      case SL_CONFIG_DESC_TAG:
        GST_DEBUG_OBJECT (qtdemux, "data %02x", QT_UINT8 (ptr));
        ptr += 1;
        break;
      default:
        GST_DEBUG_OBJECT (qtdemux, "Unknown/unhandled descriptor tag %02x",
            tag);
        GST_MEMDUMP_OBJECT (qtdemux, "descriptor data", ptr, len);
        ptr += len;
        break;
    }
  }

  /* object_type_id in the esds atom in mp4a and mp4v tells us which codec is
   * in use, and should also be used to override some other parameters for some
   * codecs. */
  switch (object_type_id) {
    case 0x20:                 /* MPEG-4 */
      /* 4 bytes for the visual_object_sequence_start_code and 1 byte for the
       * profile_and_level_indication */
      if (data_ptr != NULL && data_len >= 5 &&
          GST_READ_UINT32_BE (data_ptr) == 0x000001b0) {
        gst_codec_utils_mpeg4video_caps_set_level_and_profile (stream->caps,
            data_ptr + 4, data_len - 4);
      }
      break;                    /* Nothing special needed here */
    case 0x21:                 /* H.264 */
      codec_name = "H.264 / AVC";
      caps = gst_caps_new_simple ("video/x-h264",
          "stream-format", G_TYPE_STRING, "avc",
          "alignment", G_TYPE_STRING, "au", NULL);
      break;
    case 0x40:                 /* AAC (any) */
    case 0x66:                 /* AAC Main */
    case 0x67:                 /* AAC LC */
    case 0x68:                 /* AAC SSR */
      /* Override channels and rate based on the codec_data, as it's often
       * wrong. */
      /* Only do so for basic setup without HE-AAC extension */
      if (data_ptr && data_len == 2) {
        guint channels, rate;

        channels = gst_codec_utils_aac_get_channels (data_ptr, data_len);
        if (channels > 0)
          stream->n_channels = channels;

        rate = gst_codec_utils_aac_get_sample_rate (data_ptr, data_len);
        if (rate > 0)
          stream->rate = rate;
      }

      /* Set level and profile if possible */
      if (data_ptr != NULL && data_len >= 2) {
        gst_codec_utils_aac_caps_set_level_and_profile (stream->caps,
            data_ptr, data_len);
      } else {
        const gchar *profile_str = NULL;
        GstBuffer *buffer;
        GstMapInfo map;
        guint8 *codec_data;
        gint rate_idx, profile;

        /* No codec_data, let's invent something.
         * FIXME: This is wrong for SBR! */

        GST_WARNING_OBJECT (qtdemux, "No codec_data for AAC available");

        buffer = gst_buffer_new_and_alloc (2);
        gst_buffer_map (buffer, &map, GST_MAP_WRITE);
        codec_data = map.data;

        rate_idx =
            gst_codec_utils_aac_get_index_from_sample_rate (stream->rate);

        switch (object_type_id) {
          case 0x66:
            profile_str = "main";
            profile = 0;
            break;
          case 0x67:
            profile_str = "lc";
            profile = 1;
            break;
          case 0x68:
            profile_str = "ssr";
            profile = 2;
            break;
          default:
            profile = 3;
            break;
        }

        codec_data[0] = ((profile + 1) << 3) | ((rate_idx & 0xE) >> 1);
        codec_data[1] = ((rate_idx & 0x1) << 7) | (stream->n_channels << 3);

        gst_buffer_unmap (buffer, &map);
        gst_caps_set_simple (stream->caps, "codec_data", GST_TYPE_BUFFER,
            buffer, NULL);
        gst_buffer_unref (buffer);

        if (profile_str) {
          gst_caps_set_simple (stream->caps, "profile", G_TYPE_STRING,
              profile_str, NULL);
        }
      }
      break;
    case 0x60:                 /* MPEG-2, various profiles */
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
      codec_name = "MPEG-2 video";
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 2,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case 0x69:                 /* MPEG-2 BC audio */
    case 0x6B:                 /* MPEG-1 audio */
      caps = gst_caps_new_simple ("audio/mpeg",
          "mpegversion", G_TYPE_INT, 1, "parsed", G_TYPE_BOOLEAN, TRUE, NULL);
      codec_name = "MPEG-1 audio";
      break;
    case 0x6A:                 /* MPEG-1 */
      codec_name = "MPEG-1 video";
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 1,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case 0x6C:                 /* MJPEG */
      caps =
          gst_caps_new_simple ("image/jpeg", "parsed", G_TYPE_BOOLEAN, TRUE,
          NULL);
      codec_name = "Motion-JPEG";
      break;
    case 0x6D:                 /* PNG */
      caps =
          gst_caps_new_simple ("image/png", "parsed", G_TYPE_BOOLEAN, TRUE,
          NULL);
      codec_name = "PNG still images";
      break;
    case 0x6E:                 /* JPEG2000 */
      codec_name = "JPEG-2000";
      caps = gst_caps_new_simple ("image/x-j2c", "fields", G_TYPE_INT, 1, NULL);
      break;
    case 0xA4:                 /* Dirac */
      codec_name = "Dirac";
      caps = gst_caps_new_empty_simple ("video/x-dirac");
      break;
    case 0xA5:                 /* AC3 */
      codec_name = "AC-3 audio";
      caps = gst_caps_new_simple ("audio/x-ac3",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      break;
    case 0xA9:                 /* AC3 */
      codec_name = "DTS audio";
      caps = gst_caps_new_simple ("audio/x-dts",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      break;
    case 0xE1:                 /* QCELP */
      /* QCELP, the codec_data is a riff tag (little endian) with
       * more info (http://ftp.3gpp2.org/TSGC/Working/2003/2003-05-SanDiego/TSG-C-2003-05-San%20Diego/WG1/SWG12/C12-20030512-006%20=%20C12-20030217-015_Draft_Baseline%20Text%20of%20FFMS_R2.doc). */
      caps = gst_caps_new_empty_simple ("audio/qcelp");
      codec_name = "QCELP";
      break;
    default:
      break;
  }

  /* If we have a replacement caps, then change our caps for this stream */
  if (caps) {
    gst_caps_unref (stream->caps);
    stream->caps = caps;
  }

  if (codec_name && list)
    gst_tag_list_add (list, GST_TAG_MERGE_REPLACE,
        GST_TAG_AUDIO_CODEC, codec_name, NULL);

  /* Add the codec_data attribute to caps, if we have it */
  if (data_ptr) {
    GstBuffer *buffer;

    buffer = gst_buffer_new_and_alloc (data_len);
    gst_buffer_fill (buffer, 0, data_ptr, data_len);

    GST_DEBUG_OBJECT (qtdemux, "setting codec_data from esds");
    GST_MEMDUMP_OBJECT (qtdemux, "codec_data from esds", data_ptr, data_len);

    gst_caps_set_simple (stream->caps, "codec_data", GST_TYPE_BUFFER,
        buffer, NULL);
    gst_buffer_unref (buffer);
  }

}