gst_aac_parse_check_loas_frame (GstAacParse * aacparse,
    const guint8 * data, const guint avail, gboolean drain,
    guint * framesize, guint * needed_data)
{
  *needed_data = 0;

  /* 3 byte header */
  if (G_UNLIKELY (avail < 3)) {
    *needed_data = 3;
    return FALSE;
  }

  if ((data[0] == 0x56) && ((data[1] & 0xe0) == 0xe0)) {
    *framesize = gst_aac_parse_loas_get_frame_len (data);
    GST_DEBUG_OBJECT (aacparse, "Found possible %u byte LOAS frame",
        *framesize);

    /* In EOS mode this is enough. No need to examine the data further.
       We also relax the check when we have sync, on the assumption that
       if we're not looking at random data, we have a much higher chance
       to get the correct sync, and this avoids losing two frames when
       a single bit corruption happens. */
    if (drain || !GST_BASE_PARSE_LOST_SYNC (aacparse)) {
      return TRUE;
    }

    if (*framesize + LOAS_MAX_SIZE > avail) {
      /* We have found a possible frame header candidate, but can't be
         sure since we don't have enough data to check the next frame */
      GST_DEBUG ("NEED MORE DATA: we need %d, available %d",
          *framesize + LOAS_MAX_SIZE, avail);
      *needed_data = *framesize + LOAS_MAX_SIZE;
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          *framesize + LOAS_MAX_SIZE);
      return FALSE;
    }

    if ((data[*framesize] == 0x56) && ((data[*framesize + 1] & 0xe0) == 0xe0)) {
      guint nextlen = gst_aac_parse_loas_get_frame_len (data + (*framesize));

      GST_LOG ("LOAS frame found, len: %d bytes", *framesize);
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          nextlen + LOAS_MAX_SIZE);
      return TRUE;
    } else {
      GST_DEBUG_OBJECT (aacparse, "That was a false positive");
    }
  }
  return FALSE;
}