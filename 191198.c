qtdemux_expose_streams (GstQTDemux * qtdemux)
{
  gint i;
  GSList *oldpads = NULL;
  GSList *iter;

  GST_DEBUG_OBJECT (qtdemux, "exposing streams");

  for (i = 0; i < qtdemux->n_streams; i++) {
    QtDemuxStream *stream = qtdemux->streams[i];
    GstPad *oldpad = stream->pad;
    GstTagList *list;

    GST_DEBUG_OBJECT (qtdemux, "stream %d, id %d, fourcc %" GST_FOURCC_FORMAT,
        i, stream->track_id, GST_FOURCC_ARGS (stream->fourcc));

    if ((stream->subtype == FOURCC_text || stream->subtype == FOURCC_sbtl) &&
        stream->track_id == qtdemux->chapters_track_id) {
      /* TODO - parse chapters track and expose it as GstToc; For now just ignore it
         so that it doesn't look like a subtitle track */
      gst_qtdemux_remove_stream (qtdemux, i);
      i--;
      continue;
    }

    /* now we have all info and can expose */
    list = stream->pending_tags;
    stream->pending_tags = NULL;
    if (oldpad)
      oldpads = g_slist_prepend (oldpads, oldpad);
    if (!gst_qtdemux_add_stream (qtdemux, stream, list))
      return GST_FLOW_ERROR;
  }

  gst_qtdemux_guess_bitrate (qtdemux);

  gst_element_no_more_pads (GST_ELEMENT_CAST (qtdemux));

  for (iter = oldpads; iter; iter = g_slist_next (iter)) {
    GstPad *oldpad = iter->data;
    GstEvent *event;

    event = gst_event_new_eos ();
    if (qtdemux->segment_seqnum)
      gst_event_set_seqnum (event, qtdemux->segment_seqnum);

    gst_pad_push_event (oldpad, event);
    gst_pad_set_active (oldpad, FALSE);
    gst_element_remove_pad (GST_ELEMENT (qtdemux), oldpad);
    gst_flow_combiner_remove_pad (qtdemux->flowcombiner, oldpad);
    gst_object_unref (oldpad);
  }

  /* check if we should post a redirect in case there is a single trak
   * and it is a redirecting trak */
  if (qtdemux->n_streams == 1 && qtdemux->streams[0]->redirect_uri != NULL) {
    GstMessage *m;

    GST_INFO_OBJECT (qtdemux, "Issuing a redirect due to a single track with "
        "an external content");
    m = gst_message_new_element (GST_OBJECT_CAST (qtdemux),
        gst_structure_new ("redirect",
            "new-location", G_TYPE_STRING, qtdemux->streams[0]->redirect_uri,
            NULL));
    gst_element_post_message (GST_ELEMENT_CAST (qtdemux), m);
    qtdemux->posted_redirect = TRUE;
  }

  for (i = 0; i < qtdemux->n_streams; i++) {
    QtDemuxStream *stream = qtdemux->streams[i];

    qtdemux_do_allocation (qtdemux, stream);
  }

  qtdemux->exposed = TRUE;
  return GST_FLOW_OK;
}