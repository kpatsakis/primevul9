gst_aac_parse_handle_frame (GstBaseParse * parse,
    GstBaseParseFrame * frame, gint * skipsize)
{
  GstMapInfo map;
  GstAacParse *aacparse;
  gboolean ret = FALSE;
  gboolean lost_sync;
  GstBuffer *buffer;
  guint framesize;
  gint rate = 0, channels = 0;

  aacparse = GST_AAC_PARSE (parse);
  buffer = frame->buffer;

  gst_buffer_map (buffer, &map, GST_MAP_READ);

  *skipsize = -1;
  lost_sync = GST_BASE_PARSE_LOST_SYNC (parse);

  if (aacparse->header_type == DSPAAC_HEADER_ADIF ||
      aacparse->header_type == DSPAAC_HEADER_NONE) {
    /* There is nothing to parse */
    framesize = map.size;
    ret = TRUE;

  } else if (aacparse->header_type == DSPAAC_HEADER_NOT_PARSED || lost_sync) {

    ret = gst_aac_parse_detect_stream (aacparse, map.data, map.size,
        GST_BASE_PARSE_DRAINING (parse), &framesize, skipsize);

  } else if (aacparse->header_type == DSPAAC_HEADER_ADTS) {
    guint needed_data = 1024;

    ret = gst_aac_parse_check_adts_frame (aacparse, map.data, map.size,
        GST_BASE_PARSE_DRAINING (parse), &framesize, &needed_data);

    if (!ret && needed_data) {
      GST_DEBUG ("buffer didn't contain valid frame");
      *skipsize = 0;
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          needed_data);
    }

  } else if (aacparse->header_type == DSPAAC_HEADER_LOAS) {
    guint needed_data = 1024;

    ret = gst_aac_parse_check_loas_frame (aacparse, map.data,
        map.size, GST_BASE_PARSE_DRAINING (parse), &framesize, &needed_data);

    if (!ret && needed_data) {
      GST_DEBUG ("buffer didn't contain valid frame");
      *skipsize = 0;
      gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
          needed_data);
    }

  } else {
    GST_DEBUG ("buffer didn't contain valid frame");
    gst_base_parse_set_min_frame_size (GST_BASE_PARSE (aacparse),
        ADTS_MAX_SIZE);
  }

  if (G_UNLIKELY (!ret))
    goto exit;

  if (aacparse->header_type == DSPAAC_HEADER_ADTS) {
    /* see above */
    frame->overhead = 7;

    gst_aac_parse_parse_adts_header (aacparse, map.data,
        &rate, &channels, NULL, NULL);

    GST_LOG_OBJECT (aacparse, "rate: %d, chans: %d", rate, channels);

    if (G_UNLIKELY (rate != aacparse->sample_rate
            || channels != aacparse->channels)) {
      aacparse->sample_rate = rate;
      aacparse->channels = channels;

      if (!gst_aac_parse_set_src_caps (aacparse, NULL)) {
        /* If linking fails, we need to return appropriate error */
        ret = GST_FLOW_NOT_LINKED;
      }

      gst_base_parse_set_frame_rate (GST_BASE_PARSE (aacparse),
          aacparse->sample_rate, aacparse->frame_samples, 2, 2);
    }
  } else if (aacparse->header_type == DSPAAC_HEADER_LOAS) {
    gboolean setcaps = FALSE;

    /* see above */
    frame->overhead = 3;

    if (!gst_aac_parse_read_loas_config (aacparse, map.data, map.size, &rate,
            &channels, NULL) || !rate || !channels) {
      /* This is pretty normal when skipping data at the start of
       * random stream (MPEG-TS capture for example) */
      GST_DEBUG_OBJECT (aacparse, "Error reading LOAS config. Skipping.");
      /* Since we don't fully parse the LOAS config, we don't know for sure
       * how much to skip. Just skip 1 to end up to the next marker and
       * resume parsing from there */
      *skipsize = 1;
      goto exit;
    }

    if (G_UNLIKELY (rate != aacparse->sample_rate
            || channels != aacparse->channels)) {
      aacparse->sample_rate = rate;
      aacparse->channels = channels;
      setcaps = TRUE;
      GST_INFO_OBJECT (aacparse, "New LOAS config: %d Hz, %d channels", rate,
          channels);
    }

    /* We want to set caps both at start, and when rate/channels change.
       Since only some LOAS frames have that info, we may receive frames
       before knowing about rate/channels. */
    if (setcaps
        || !gst_pad_has_current_caps (GST_BASE_PARSE_SRC_PAD (aacparse))) {
      if (!gst_aac_parse_set_src_caps (aacparse, NULL)) {
        /* If linking fails, we need to return appropriate error */
        ret = GST_FLOW_NOT_LINKED;
      }

      gst_base_parse_set_frame_rate (GST_BASE_PARSE (aacparse),
          aacparse->sample_rate, aacparse->frame_samples, 2, 2);
    }
  }

  if (aacparse->header_type == DSPAAC_HEADER_NONE
      && aacparse->output_header_type == DSPAAC_HEADER_ADTS) {
    if (!gst_aac_parse_prepend_adts_headers (aacparse, frame)) {
      GST_ERROR_OBJECT (aacparse, "Failed to prepend ADTS headers to frame");
      ret = GST_FLOW_ERROR;
    }
  }

exit:
  gst_buffer_unmap (buffer, &map);

  if (ret) {
    /* found, skip if needed */
    if (*skipsize > 0)
      return GST_FLOW_OK;
    *skipsize = 0;
  } else {
    if (*skipsize < 0)
      *skipsize = 1;
  }

  if (ret && framesize <= map.size) {
    return gst_base_parse_finish_frame (parse, frame, framesize);
  }

  return GST_FLOW_OK;
}