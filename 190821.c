gst_aac_parse_check_adts_frame (GstAacParse * aacparse,
    const guint8 * data, const guint avail, gboolean drain,
    guint * framesize, guint * needed_data)
{
  guint crc_size;

  *needed_data = 0;

  /* Absolute minimum to perform the ADTS syncword,
     layer and sampling frequency tests */
  if (G_UNLIKELY (avail < 3)) {
    *needed_data = 3;
    return FALSE;
  }

  /* Syncword and layer tests */
  if ((data[0] == 0xff) && ((data[1] & 0xf6) == 0xf0)) {

    /* Sampling frequency test */
    if (G_UNLIKELY ((data[2] & 0x3C) >> 2 == 15))
      return FALSE;

    /* This looks like an ADTS frame header but
       we need at least 6 bytes to proceed */
    if (G_UNLIKELY (avail < 6)) {
      *needed_data = 6;
      return FALSE;
    }

    *framesize = gst_aac_parse_adts_get_frame_len (data);

    /* If frame has CRC, it needs 2 bytes
       for it at the end of the header */
    crc_size = (data[1] & 0x01) ? 0 : 2;

    /* CRC size test */
    if (*framesize < 7 + crc_size) {
      *needed_data = 7 + crc_size;
      return FALSE;
    }

    /* In EOS mode this is enough. No need to examine the data further.
       We also relax the check when we have sync, on the assumption that
       if we're not looking at random data, we have a much higher chance
       to get the correct sync, and this avoids losing two frames when
       a single bit corruption happens. */
    if (drain || !GST_BASE_PARSE_LOST_SYNC (aacparse)) {
      return TRUE;
    }

    if (*framesize + ADTS_MAX_SIZE > avail) {
      /* We have found a possible frame header candidate, but can't be
         sure since we don't have enough data to check the next frame */
      GST_DEBUG ("NEED MORE DATA: we need %d, available %d",
          *framesize + ADTS_MAX_SIZE, avail);
      *needed_data = *framesize + ADTS_MAX_SIZE;
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          *framesize + ADTS_MAX_SIZE);
      return FALSE;
    }

    if ((data[*framesize] == 0xff) && ((data[*framesize + 1] & 0xf6) == 0xf0)) {
      guint nextlen = gst_aac_parse_adts_get_frame_len (data + (*framesize));

      GST_LOG ("ADTS frame found, len: %d bytes", *framesize);
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          nextlen + ADTS_MAX_SIZE);
      return TRUE;
    }
  }
  return FALSE;
}