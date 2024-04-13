gst_matroska_demux_send_tags (GstMatroskaDemux * demux)
{
  gint i;

  if (G_UNLIKELY (demux->common.global_tags_changed)) {
    GstEvent *tag_event;
    gst_tag_list_add (demux->common.global_tags, GST_TAG_MERGE_REPLACE,
        GST_TAG_CONTAINER_FORMAT, "Matroska", NULL);
    GST_DEBUG_OBJECT (demux, "Sending global_tags %p : %" GST_PTR_FORMAT,
        demux->common.global_tags, demux->common.global_tags);

    tag_event =
        gst_event_new_tag (gst_tag_list_copy (demux->common.global_tags));

    for (i = 0; i < demux->common.src->len; i++) {
      GstMatroskaTrackContext *stream;

      stream = g_ptr_array_index (demux->common.src, i);
      gst_pad_push_event (stream->pad, gst_event_ref (tag_event));
    }

    gst_event_unref (tag_event);
    demux->common.global_tags_changed = FALSE;
  }

  g_assert (demux->common.src->len == demux->common.num_streams);
  for (i = 0; i < demux->common.src->len; i++) {
    GstMatroskaTrackContext *stream;

    stream = g_ptr_array_index (demux->common.src, i);

    if (G_UNLIKELY (stream->tags_changed)) {
      GST_DEBUG_OBJECT (demux, "Sending tags %p for pad %s:%s : %"
          GST_PTR_FORMAT, stream->tags,
          GST_DEBUG_PAD_NAME (stream->pad), stream->tags);
      gst_pad_push_event (stream->pad,
          gst_event_new_tag (gst_tag_list_copy (stream->tags)));
      stream->tags_changed = FALSE;
    }
  }
}