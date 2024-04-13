qtdemux_do_allocation (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
/* FIXME: This can only reliably work if demuxers have a
 * separate streaming thread per srcpad. This should be
 * done in a demuxer base class, which integrates parts
 * of multiqueue
 *
 * https://bugzilla.gnome.org/show_bug.cgi?id=701856
 */
#if 0
  GstQuery *query;

  query = gst_query_new_allocation (stream->caps, FALSE);

  if (!gst_pad_peer_query (stream->pad, query)) {
    /* not a problem, just debug a little */
    GST_DEBUG_OBJECT (qtdemux, "peer ALLOCATION query failed");
  }

  if (stream->allocator)
    gst_object_unref (stream->allocator);

  if (gst_query_get_n_allocation_params (query) > 0) {
    /* try the allocator */
    gst_query_parse_nth_allocation_param (query, 0, &stream->allocator,
        &stream->params);
    stream->use_allocator = TRUE;
  } else {
    stream->allocator = NULL;
    gst_allocation_params_init (&stream->params);
    stream->use_allocator = FALSE;
  }
  gst_query_unref (query);
#endif
}