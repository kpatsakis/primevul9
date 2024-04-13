gst_qtdemux_decorate_and_push_buffer (GstQTDemux * qtdemux,
    QtDemuxStream * stream, GstBuffer * buf,
    GstClockTime dts, GstClockTime pts, GstClockTime duration,
    gboolean keyframe, GstClockTime position, guint64 byte_position)
{
  GstFlowReturn ret = GST_FLOW_OK;

  /* offset the timestamps according to the edit list */

  if (G_UNLIKELY (stream->fourcc == FOURCC_rtsp)) {
    gchar *url;
    GstMapInfo map;

    gst_buffer_map (buf, &map, GST_MAP_READ);
    url = g_strndup ((gchar *) map.data, map.size);
    gst_buffer_unmap (buf, &map);
    if (url != NULL && strlen (url) != 0) {
      /* we have RTSP redirect now */
      gst_element_post_message (GST_ELEMENT_CAST (qtdemux),
          gst_message_new_element (GST_OBJECT_CAST (qtdemux),
              gst_structure_new ("redirect",
                  "new-location", G_TYPE_STRING, url, NULL)));
      qtdemux->posted_redirect = TRUE;
    } else {
      GST_WARNING_OBJECT (qtdemux, "Redirect URI of stream is empty, not "
          "posting");
    }
    g_free (url);
  }

  /* position reporting */
  if (qtdemux->segment.rate >= 0) {
    qtdemux->segment.position = position;
    gst_qtdemux_sync_streams (qtdemux);
  }

  if (G_UNLIKELY (!stream->pad)) {
    GST_DEBUG_OBJECT (qtdemux, "No output pad for stream, ignoring");
    gst_buffer_unref (buf);
    goto exit;
  }

  /* send out pending buffers */
  while (stream->buffers) {
    GstBuffer *buffer = (GstBuffer *) stream->buffers->data;

    if (G_UNLIKELY (stream->discont)) {
      GST_LOG_OBJECT (qtdemux, "marking discont buffer");
      GST_BUFFER_FLAG_SET (buffer, GST_BUFFER_FLAG_DISCONT);
      stream->discont = FALSE;
    } else {
      GST_BUFFER_FLAG_UNSET (buf, GST_BUFFER_FLAG_DISCONT);
    }

    if (stream->alignment > 1)
      buffer = gst_qtdemux_align_buffer (qtdemux, buffer, stream->alignment);
    gst_pad_push (stream->pad, buffer);

    stream->buffers = g_slist_delete_link (stream->buffers, stream->buffers);
  }

  /* we're going to modify the metadata */
  buf = gst_buffer_make_writable (buf);

  if (G_UNLIKELY (stream->need_process))
    buf = gst_qtdemux_process_buffer (qtdemux, stream, buf);

  if (!buf) {
    goto exit;
  }

  GST_BUFFER_DTS (buf) = dts;
  GST_BUFFER_PTS (buf) = pts;
  GST_BUFFER_DURATION (buf) = duration;
  GST_BUFFER_OFFSET (buf) = -1;
  GST_BUFFER_OFFSET_END (buf) = -1;

  if (G_UNLIKELY (stream->rgb8_palette))
    gst_buffer_append_memory (buf, gst_memory_ref (stream->rgb8_palette));

  if (G_UNLIKELY (stream->padding)) {
    gst_buffer_resize (buf, stream->padding, -1);
  }
#if 0
  if (G_UNLIKELY (qtdemux->element_index)) {
    GstClockTime stream_time;

    stream_time =
        gst_segment_to_stream_time (&stream->segment, GST_FORMAT_TIME,
        timestamp);
    if (GST_CLOCK_TIME_IS_VALID (stream_time)) {
      GST_LOG_OBJECT (qtdemux,
          "adding association %" GST_TIME_FORMAT "-> %"
          G_GUINT64_FORMAT, GST_TIME_ARGS (stream_time), byte_position);
      gst_index_add_association (qtdemux->element_index,
          qtdemux->index_id,
          keyframe ? GST_ASSOCIATION_FLAG_KEY_UNIT :
          GST_ASSOCIATION_FLAG_DELTA_UNIT, GST_FORMAT_TIME, stream_time,
          GST_FORMAT_BYTES, byte_position, NULL);
    }
  }
#endif

  if (stream->need_clip)
    buf = gst_qtdemux_clip_buffer (qtdemux, stream, buf);

  if (G_UNLIKELY (buf == NULL))
    goto exit;

  if (G_UNLIKELY (stream->discont)) {
    GST_LOG_OBJECT (qtdemux, "marking discont buffer");
    GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_DISCONT);
    stream->discont = FALSE;
  } else {
    GST_BUFFER_FLAG_UNSET (buf, GST_BUFFER_FLAG_DISCONT);
  }

  if (!keyframe) {
    GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_DELTA_UNIT);
    stream->on_keyframe = FALSE;
  } else {
    stream->on_keyframe = TRUE;
  }


  GST_LOG_OBJECT (qtdemux,
      "Pushing buffer with dts %" GST_TIME_FORMAT ", pts %" GST_TIME_FORMAT
      ", duration %" GST_TIME_FORMAT " on pad %s", GST_TIME_ARGS (dts),
      GST_TIME_ARGS (pts), GST_TIME_ARGS (duration),
      GST_PAD_NAME (stream->pad));

  if (stream->protected && stream->protection_scheme_type == FOURCC_cenc) {
    GstStructure *crypto_info;
    QtDemuxCencSampleSetInfo *info =
        (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;
    gint index;
    GstEvent *event;

    while ((event = g_queue_pop_head (&stream->protection_scheme_event_queue))) {
      gst_pad_push_event (stream->pad, event);
    }

    if (info->crypto_info == NULL) {
      GST_DEBUG_OBJECT (qtdemux, "cenc metadata hasn't been parsed yet");
      gst_buffer_unref (buf);
      goto exit;
    }

    index = stream->sample_index - (stream->n_samples - info->crypto_info->len);
    if (G_LIKELY (index >= 0 && index < info->crypto_info->len)) {
      /* steal structure from array */
      crypto_info = g_ptr_array_index (info->crypto_info, index);
      g_ptr_array_index (info->crypto_info, index) = NULL;
      GST_LOG_OBJECT (qtdemux, "attaching cenc metadata [%u]", index);
      if (!crypto_info || !gst_buffer_add_protection_meta (buf, crypto_info))
        GST_ERROR_OBJECT (qtdemux, "failed to attach cenc metadata to buffer");
    }
  }

  if (stream->alignment > 1)
    buf = gst_qtdemux_align_buffer (qtdemux, buf, stream->alignment);

  ret = gst_pad_push (stream->pad, buf);

  if (GST_CLOCK_TIME_IS_VALID (pts) && GST_CLOCK_TIME_IS_VALID (duration)) {
    /* mark position in stream, we'll need this to know when to send GAP event */
    stream->segment.position = pts + duration;
  }

exit:
  return ret;
}