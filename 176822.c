gst_matroska_demux_parse_contents_seekentry (GstMatroskaDemux * demux,
    GstEbmlRead * ebml)
{
  GstFlowReturn ret;
  guint64 seek_pos = (guint64) - 1;
  guint32 seek_id = 0;
  guint32 id;

  DEBUG_ELEMENT_START (demux, ebml, "Seek");

  if ((ret = gst_ebml_read_master (ebml, &id)) != GST_FLOW_OK) {
    DEBUG_ELEMENT_STOP (demux, ebml, "Seek", ret);
    return ret;
  }

  while (ret == GST_FLOW_OK && gst_ebml_read_has_remaining (ebml, 1, TRUE)) {
    if ((ret = gst_ebml_peek_id (ebml, &id)) != GST_FLOW_OK)
      break;

    switch (id) {
      case GST_MATROSKA_ID_SEEKID:
      {
        guint64 t;

        if ((ret = gst_ebml_read_uint (ebml, &id, &t)) != GST_FLOW_OK)
          break;

        GST_DEBUG_OBJECT (demux, "SeekID: %" G_GUINT64_FORMAT, t);
        seek_id = t;
        break;
      }

      case GST_MATROSKA_ID_SEEKPOSITION:
      {
        guint64 t;

        if ((ret = gst_ebml_read_uint (ebml, &id, &t)) != GST_FLOW_OK)
          break;

        if (t > G_MAXINT64) {
          GST_WARNING_OBJECT (demux,
              "Too large SeekPosition %" G_GUINT64_FORMAT, t);
          break;
        }

        GST_DEBUG_OBJECT (demux, "SeekPosition: %" G_GUINT64_FORMAT, t);
        seek_pos = t;
        break;
      }

      default:
        ret = gst_matroska_read_common_parse_skip (&demux->common, ebml,
            "SeekHead", id);
        break;
    }
  }

  if (ret != GST_FLOW_OK && ret != GST_FLOW_EOS)
    return ret;

  if (!seek_id || seek_pos == (guint64) - 1) {
    GST_WARNING_OBJECT (demux, "Incomplete seekhead entry (0x%x/%"
        G_GUINT64_FORMAT ")", seek_id, seek_pos);
    return GST_FLOW_OK;
  }

  switch (seek_id) {
    case GST_MATROSKA_ID_SEEKHEAD:
    {
    }
    case GST_MATROSKA_ID_CUES:
    case GST_MATROSKA_ID_TAGS:
    case GST_MATROSKA_ID_TRACKS:
    case GST_MATROSKA_ID_SEGMENTINFO:
    case GST_MATROSKA_ID_ATTACHMENTS:
    case GST_MATROSKA_ID_CHAPTERS:
    {
      guint64 before_pos, length;
      guint needed;

      /* remember */
      length = gst_matroska_read_common_get_length (&demux->common);
      before_pos = demux->common.offset;

      if (length == (guint64) - 1) {
        GST_DEBUG_OBJECT (demux, "no upstream length, skipping SeakHead entry");
        break;
      }

      /* check for validity */
      if (seek_pos + demux->common.ebml_segment_start + 12 >= length) {
        GST_WARNING_OBJECT (demux,
            "SeekHead reference lies outside file!" " (%"
            G_GUINT64_FORMAT "+%" G_GUINT64_FORMAT "+12 >= %"
            G_GUINT64_FORMAT ")", seek_pos, demux->common.ebml_segment_start,
            length);
        break;
      }

      /* only pick up index location when streaming */
      if (demux->streaming) {
        if (seek_id == GST_MATROSKA_ID_CUES) {
          demux->index_offset = seek_pos + demux->common.ebml_segment_start;
          GST_DEBUG_OBJECT (demux, "Cues located at offset %" G_GUINT64_FORMAT,
              demux->index_offset);
        }
        break;
      }

      /* seek */
      demux->common.offset = seek_pos + demux->common.ebml_segment_start;

      /* check ID */
      if ((ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
                  GST_ELEMENT_CAST (demux), &id, &length, &needed)) !=
          GST_FLOW_OK)
        goto finish;

      if (id != seek_id) {
        GST_WARNING_OBJECT (demux,
            "We looked for ID=0x%x but got ID=0x%x (pos=%" G_GUINT64_FORMAT ")",
            seek_id, id, seek_pos + demux->common.ebml_segment_start);
      } else {
        /* now parse */
        ret = gst_matroska_demux_parse_id (demux, id, length, needed);
      }

    finish:
      /* seek back */
      demux->common.offset = before_pos;
      break;
    }

    case GST_MATROSKA_ID_CLUSTER:
    {
      guint64 pos = seek_pos + demux->common.ebml_segment_start;

      GST_LOG_OBJECT (demux, "Cluster position");
      if (G_UNLIKELY (!demux->clusters))
        demux->clusters = g_array_sized_new (TRUE, TRUE, sizeof (guint64), 100);
      g_array_append_val (demux->clusters, pos);
      break;
    }

    default:
      GST_DEBUG_OBJECT (demux, "Ignoring Seek entry for ID=0x%x", seek_id);
      break;
  }
  DEBUG_ELEMENT_STOP (demux, ebml, "Seek", ret);

  return ret;
}