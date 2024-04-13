gst_qtdemux_stream_clear (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
  if (stream->allocator)
    gst_object_unref (stream->allocator);
  while (stream->buffers) {
    gst_buffer_unref (GST_BUFFER_CAST (stream->buffers->data));
    stream->buffers = g_slist_delete_link (stream->buffers, stream->buffers);
  }
  if (stream->rgb8_palette) {
    gst_memory_unref (stream->rgb8_palette);
    stream->rgb8_palette = NULL;
  }

  if (stream->pending_tags)
    gst_tag_list_unref (stream->pending_tags);
  stream->pending_tags = NULL;
  g_free (stream->redirect_uri);
  stream->redirect_uri = NULL;
  stream->sent_eos = FALSE;
  stream->sparse = FALSE;
  stream->protected = FALSE;
  if (stream->protection_scheme_info) {
    if (stream->protection_scheme_type == FOURCC_cenc) {
      QtDemuxCencSampleSetInfo *info =
          (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;
      if (info->default_properties)
        gst_structure_free (info->default_properties);
      if (info->crypto_info)
        g_ptr_array_free (info->crypto_info, TRUE);
    }
    g_free (stream->protection_scheme_info);
    stream->protection_scheme_info = NULL;
  }
  stream->protection_scheme_type = 0;
  stream->protection_scheme_version = 0;
  g_queue_foreach (&stream->protection_scheme_event_queue,
      (GFunc) gst_event_unref, NULL);
  g_queue_clear (&stream->protection_scheme_event_queue);
  gst_qtdemux_stream_flush_segments_data (qtdemux, stream);
  gst_qtdemux_stream_flush_samples_data (qtdemux, stream);
}