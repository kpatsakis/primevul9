gst_aac_parse_detect_stream (GstAacParse * aacparse,
    const guint8 * data, const guint avail, gboolean drain,
    guint * framesize, gint * skipsize)
{
  gboolean found = FALSE;
  guint need_data_adts = 0, need_data_loas;
  guint i = 0;

  GST_DEBUG_OBJECT (aacparse, "Parsing header data");

  /* FIXME: No need to check for ADIF if we are not in the beginning of the
     stream */

  /* Can we even parse the header? */
  if (avail < MAX (ADTS_MAX_SIZE, LOAS_MAX_SIZE)) {
    GST_DEBUG_OBJECT (aacparse, "Not enough data to check");
    return FALSE;
  }

  for (i = 0; i < avail - 4; i++) {
    if (((data[i] == 0xff) && ((data[i + 1] & 0xf6) == 0xf0)) ||
        ((data[i] == 0x56) && ((data[i + 1] & 0xe0) == 0xe0)) ||
        strncmp ((char *) data + i, "ADIF", 4) == 0) {
      GST_DEBUG_OBJECT (aacparse, "Found signature at offset %u", i);
      found = TRUE;

      if (i) {
        /* Trick: tell the parent class that we didn't find the frame yet,
           but make it skip 'i' amount of bytes. Next time we arrive
           here we have full frame in the beginning of the data. */
        *skipsize = i;
        return FALSE;
      }
      break;
    }
  }
  if (!found) {
    if (i)
      *skipsize = i;
    return FALSE;
  }

  if (gst_aac_parse_check_adts_frame (aacparse, data, avail, drain,
          framesize, &need_data_adts)) {
    gint rate, channels;

    GST_INFO ("ADTS ID: %d, framesize: %d", (data[1] & 0x08) >> 3, *framesize);

    gst_aac_parse_parse_adts_header (aacparse, data, &rate, &channels,
        &aacparse->object_type, &aacparse->mpegversion);

    if (!channels || !framesize) {
      GST_DEBUG_OBJECT (aacparse, "impossible ADTS configuration");
      return FALSE;
    }

    aacparse->header_type = DSPAAC_HEADER_ADTS;
    gst_base_parse_set_frame_rate (GST_BASE_PARSE (aacparse), rate,
        aacparse->frame_samples, 2, 2);

    GST_DEBUG ("ADTS: samplerate %d, channels %d, objtype %d, version %d",
        rate, channels, aacparse->object_type, aacparse->mpegversion);

    gst_base_parse_set_syncable (GST_BASE_PARSE (aacparse), TRUE);

    return TRUE;
  }

  if (gst_aac_parse_check_loas_frame (aacparse, data, avail, drain,
          framesize, &need_data_loas)) {
    gint rate = 0, channels = 0;

    GST_INFO ("LOAS, framesize: %d", *framesize);

    aacparse->header_type = DSPAAC_HEADER_LOAS;

    if (!gst_aac_parse_read_loas_config (aacparse, data, avail, &rate,
            &channels, &aacparse->mpegversion)) {
      /* This is pretty normal when skipping data at the start of
       * random stream (MPEG-TS capture for example) */
      GST_LOG_OBJECT (aacparse, "Error reading LOAS config");
      return FALSE;
    }

    if (rate && channels) {
      gst_base_parse_set_frame_rate (GST_BASE_PARSE (aacparse), rate,
          aacparse->frame_samples, 2, 2);

      /* Don't store the sample rate and channels yet -
       * this is just format detection. */
      GST_DEBUG ("LOAS: samplerate %d, channels %d, objtype %d, version %d",
          rate, channels, aacparse->object_type, aacparse->mpegversion);
    }

    gst_base_parse_set_syncable (GST_BASE_PARSE (aacparse), TRUE);

    return TRUE;
  }

  if (need_data_adts || need_data_loas) {
    /* This tells the parent class not to skip any data */
    *skipsize = 0;
    return FALSE;
  }

  if (avail < ADIF_MAX_SIZE)
    return FALSE;

  if (memcmp (data + i, "ADIF", 4) == 0) {
    const guint8 *adif;
    int skip_size = 0;
    int bitstream_type;
    int sr_idx;
    GstCaps *sinkcaps;

    aacparse->header_type = DSPAAC_HEADER_ADIF;
    aacparse->mpegversion = 4;

    /* Skip the "ADIF" bytes */
    adif = data + i + 4;

    /* copyright string */
    if (adif[0] & 0x80)
      skip_size += 9;           /* skip 9 bytes */

    bitstream_type = adif[0 + skip_size] & 0x10;
    aacparse->bitrate =
        ((unsigned int) (adif[0 + skip_size] & 0x0f) << 19) |
        ((unsigned int) adif[1 + skip_size] << 11) |
        ((unsigned int) adif[2 + skip_size] << 3) |
        ((unsigned int) adif[3 + skip_size] & 0xe0);

    /* CBR */
    if (bitstream_type == 0) {
#if 0
      /* Buffer fullness parsing. Currently not needed... */
      guint num_elems = 0;
      guint fullness = 0;

      num_elems = (adif[3 + skip_size] & 0x1e);
      GST_INFO ("ADIF num_config_elems: %d", num_elems);

      fullness = ((unsigned int) (adif[3 + skip_size] & 0x01) << 19) |
          ((unsigned int) adif[4 + skip_size] << 11) |
          ((unsigned int) adif[5 + skip_size] << 3) |
          ((unsigned int) (adif[6 + skip_size] & 0xe0) >> 5);

      GST_INFO ("ADIF buffer fullness: %d", fullness);
#endif
      aacparse->object_type = ((adif[6 + skip_size] & 0x01) << 1) |
          ((adif[7 + skip_size] & 0x80) >> 7);
      sr_idx = (adif[7 + skip_size] & 0x78) >> 3;
    }
    /* VBR */
    else {
      aacparse->object_type = (adif[4 + skip_size] & 0x18) >> 3;
      sr_idx = ((adif[4 + skip_size] & 0x07) << 1) |
          ((adif[5 + skip_size] & 0x80) >> 7);
    }

    /* FIXME: This gives totally wrong results. Duration calculation cannot
       be based on this */
    aacparse->sample_rate =
        gst_codec_utils_aac_get_sample_rate_from_index (sr_idx);

    /* baseparse is not given any fps,
     * so it will give up on timestamps, seeking, etc */

    /* FIXME: Can we assume this? */
    aacparse->channels = 2;

    GST_INFO ("ADIF: br=%d, samplerate=%d, objtype=%d",
        aacparse->bitrate, aacparse->sample_rate, aacparse->object_type);

    gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse), 512);

    /* arrange for metadata and get out of the way */
    sinkcaps = gst_pad_get_current_caps (GST_BASE_PARSE_SINK_PAD (aacparse));
    gst_aac_parse_set_src_caps (aacparse, sinkcaps);
    if (sinkcaps)
      gst_caps_unref (sinkcaps);

    /* not syncable, not easily seekable (unless we push data from start */
    gst_base_parse_set_syncable (GST_BASE_PARSE_CAST (aacparse), FALSE);
    gst_base_parse_set_passthrough (GST_BASE_PARSE_CAST (aacparse), TRUE);
    gst_base_parse_set_average_bitrate (GST_BASE_PARSE_CAST (aacparse), 0);

    *framesize = avail;
    return TRUE;
  }

  /* This should never happen */
  return FALSE;
}