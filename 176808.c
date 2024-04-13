gst_matroska_demux_add_stream (GstMatroskaDemux * demux,
    GstMatroskaTrackContext * context)
{
  GstElementClass *klass = GST_ELEMENT_GET_CLASS (demux);
  gchar *padname = NULL;
  GstPadTemplate *templ = NULL;
  GstStreamFlags stream_flags;

  GstEvent *stream_start;

  gchar *stream_id;

  g_ptr_array_add (demux->common.src, context);
  context->index = demux->common.num_streams++;
  g_assert (demux->common.src->len == demux->common.num_streams);
  g_ptr_array_index (demux->common.src, demux->common.num_streams - 1) =
      context;

  /* now create the GStreamer connectivity */
  switch (context->type) {
    case GST_MATROSKA_TRACK_TYPE_VIDEO:
      padname = g_strdup_printf ("video_%u", demux->num_v_streams++);
      templ = gst_element_class_get_pad_template (klass, "video_%u");

      if (!context->intra_only)
        demux->have_nonintraonly_v_streams = TRUE;
      break;

    case GST_MATROSKA_TRACK_TYPE_AUDIO:
      padname = g_strdup_printf ("audio_%u", demux->num_a_streams++);
      templ = gst_element_class_get_pad_template (klass, "audio_%u");
      break;

    case GST_MATROSKA_TRACK_TYPE_SUBTITLE:
      padname = g_strdup_printf ("subtitle_%u", demux->num_t_streams++);
      templ = gst_element_class_get_pad_template (klass, "subtitle_%u");
      break;

    default:
      /* we should already have quit by now */
      g_assert_not_reached ();
  }

  /* the pad in here */
  context->pad = gst_pad_new_from_template (templ, padname);

  gst_pad_set_event_function (context->pad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_handle_src_event));
  gst_pad_set_query_function (context->pad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_handle_src_query));

  GST_INFO_OBJECT (demux, "Adding pad '%s' with caps %" GST_PTR_FORMAT,
      padname, context->caps);

  gst_pad_set_element_private (context->pad, context);

  gst_pad_use_fixed_caps (context->pad);
  gst_pad_set_active (context->pad, TRUE);

  stream_id =
      gst_pad_create_stream_id_printf (context->pad, GST_ELEMENT_CAST (demux),
      "%03" G_GUINT64_FORMAT ":%03" G_GUINT64_FORMAT,
      context->num, context->uid);
  stream_start =
      gst_pad_get_sticky_event (demux->common.sinkpad, GST_EVENT_STREAM_START,
      0);
  if (stream_start) {
    if (gst_event_parse_group_id (stream_start, &demux->group_id))
      demux->have_group_id = TRUE;
    else
      demux->have_group_id = FALSE;
    gst_event_unref (stream_start);
  } else if (!demux->have_group_id) {
    demux->have_group_id = TRUE;
    demux->group_id = gst_util_group_id_next ();
  }

  stream_start = gst_event_new_stream_start (stream_id);
  g_free (stream_id);
  if (demux->have_group_id)
    gst_event_set_group_id (stream_start, demux->group_id);
  stream_flags = GST_STREAM_FLAG_NONE;
  if (context->type == GST_MATROSKA_TRACK_TYPE_SUBTITLE)
    stream_flags |= GST_STREAM_FLAG_SPARSE;
  if (context->flags & GST_MATROSKA_TRACK_DEFAULT)
    stream_flags |= GST_STREAM_FLAG_SELECT;
  else if (!(context->flags & GST_MATROSKA_TRACK_ENABLED))
    stream_flags |= GST_STREAM_FLAG_UNSELECT;

  gst_event_set_stream_flags (stream_start, stream_flags);
  gst_pad_push_event (context->pad, stream_start);
  gst_pad_set_caps (context->pad, context->caps);


  if (demux->common.global_tags) {
    GstEvent *tag_event;

    gst_tag_list_add (demux->common.global_tags, GST_TAG_MERGE_REPLACE,
        GST_TAG_CONTAINER_FORMAT, "Matroska", NULL);
    GST_DEBUG_OBJECT (context->pad, "Sending global_tags %p: %" GST_PTR_FORMAT,
        demux->common.global_tags, demux->common.global_tags);

    tag_event =
        gst_event_new_tag (gst_tag_list_copy (demux->common.global_tags));

    gst_pad_push_event (context->pad, tag_event);
  }

  if (G_UNLIKELY (context->tags_changed)) {
    GST_DEBUG_OBJECT (context->pad, "Sending tags %p: %"
        GST_PTR_FORMAT, context->tags, context->tags);
    gst_pad_push_event (context->pad,
        gst_event_new_tag (gst_tag_list_copy (context->tags)));
    context->tags_changed = FALSE;
  }

  gst_element_add_pad (GST_ELEMENT (demux), context->pad);
  gst_flow_combiner_add_pad (demux->flowcombiner, context->pad);

  g_free (padname);
}