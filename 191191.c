gst_qtdemux_push_tags (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
  if (G_LIKELY (stream->pad)) {
    GST_DEBUG_OBJECT (qtdemux, "Checking pad %s:%s for tags",
        GST_DEBUG_PAD_NAME (stream->pad));

    if (G_UNLIKELY (stream->pending_tags)) {
      GST_DEBUG_OBJECT (qtdemux, "Sending tags %" GST_PTR_FORMAT,
          stream->pending_tags);
      gst_pad_push_event (stream->pad,
          gst_event_new_tag (stream->pending_tags));
      stream->pending_tags = NULL;
    }

    if (G_UNLIKELY (stream->send_global_tags && qtdemux->tag_list)) {
      GST_DEBUG_OBJECT (qtdemux, "Sending global tags %" GST_PTR_FORMAT,
          qtdemux->tag_list);
      gst_pad_push_event (stream->pad,
          gst_event_new_tag (gst_tag_list_ref (qtdemux->tag_list)));
      stream->send_global_tags = FALSE;
    }
  }
}