gst_qtdemux_add_stream (GstQTDemux * qtdemux,
    QtDemuxStream * stream, GstTagList * list)
{
  gboolean ret = TRUE;
  /* consistent default for push based mode */
  gst_segment_init (&stream->segment, GST_FORMAT_TIME);

  if (stream->subtype == FOURCC_vide) {
    gchar *name = g_strdup_printf ("video_%u", qtdemux->n_video_streams);

    stream->pad =
        gst_pad_new_from_static_template (&gst_qtdemux_videosrc_template, name);
    g_free (name);

    if (!gst_qtdemux_configure_stream (qtdemux, stream)) {
      gst_object_unref (stream->pad);
      stream->pad = NULL;
      ret = FALSE;
      goto done;
    }

    qtdemux->n_video_streams++;
  } else if (stream->subtype == FOURCC_soun) {
    gchar *name = g_strdup_printf ("audio_%u", qtdemux->n_audio_streams);

    stream->pad =
        gst_pad_new_from_static_template (&gst_qtdemux_audiosrc_template, name);
    g_free (name);
    if (!gst_qtdemux_configure_stream (qtdemux, stream)) {
      gst_object_unref (stream->pad);
      stream->pad = NULL;
      ret = FALSE;
      goto done;
    }
    qtdemux->n_audio_streams++;
  } else if (stream->subtype == FOURCC_strm) {
    GST_DEBUG_OBJECT (qtdemux, "stream type, not creating pad");
  } else if (stream->subtype == FOURCC_subp || stream->subtype == FOURCC_text
      || stream->subtype == FOURCC_sbtl || stream->subtype == FOURCC_subt) {
    gchar *name = g_strdup_printf ("subtitle_%u", qtdemux->n_sub_streams);

    stream->pad =
        gst_pad_new_from_static_template (&gst_qtdemux_subsrc_template, name);
    g_free (name);
    if (!gst_qtdemux_configure_stream (qtdemux, stream)) {
      gst_object_unref (stream->pad);
      stream->pad = NULL;
      ret = FALSE;
      goto done;
    }
    qtdemux->n_sub_streams++;
  } else if (stream->caps) {
    gchar *name = g_strdup_printf ("video_%u", qtdemux->n_video_streams);

    stream->pad =
        gst_pad_new_from_static_template (&gst_qtdemux_videosrc_template, name);
    g_free (name);
    if (!gst_qtdemux_configure_stream (qtdemux, stream)) {
      gst_object_unref (stream->pad);
      stream->pad = NULL;
      ret = FALSE;
      goto done;
    }
    qtdemux->n_video_streams++;
  } else {
    GST_DEBUG_OBJECT (qtdemux, "unknown stream type");
    goto done;
  }

  if (stream->pad) {
    GList *l;

    GST_DEBUG_OBJECT (qtdemux, "adding pad %s %p to qtdemux %p",
        GST_OBJECT_NAME (stream->pad), stream->pad, qtdemux);
    gst_element_add_pad (GST_ELEMENT_CAST (qtdemux), stream->pad);
    gst_flow_combiner_add_pad (qtdemux->flowcombiner, stream->pad);

    if (stream->pending_tags)
      gst_tag_list_unref (stream->pending_tags);
    stream->pending_tags = list;
    list = NULL;
    /* global tags go on each pad anyway */
    stream->send_global_tags = TRUE;
    /* send upstream GST_EVENT_PROTECTION events that were received before
       this source pad was created */
    for (l = qtdemux->protection_event_queue.head; l != NULL; l = l->next)
      gst_pad_push_event (stream->pad, gst_event_ref (l->data));
  }
done:
  if (list)
    gst_tag_list_unref (list);
  return ret;
}