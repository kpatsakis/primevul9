gst_matroska_demux_search_cluster (GstMatroskaDemux * demux, gint64 * pos,
    gboolean forward)
{
  gint64 newpos = *pos;
  gint64 orig_offset;
  GstFlowReturn ret = GST_FLOW_OK;
  const guint chunk = 128 * 1024;
  GstBuffer *buf = NULL;
  GstMapInfo map;
  gpointer data = NULL;
  gsize size;
  guint64 length;
  guint32 id;
  guint needed;
  gint64 oldpos, oldlength;

  orig_offset = demux->common.offset;

  GST_LOG_OBJECT (demux, "searching cluster %s offset %" G_GINT64_FORMAT,
      forward ? "following" : "preceding", *pos);

  if (demux->clusters) {
    gint64 *cpos;

    cpos = gst_util_array_binary_search (demux->clusters->data,
        demux->clusters->len, sizeof (gint64),
        (GCompareDataFunc) gst_matroska_cluster_compare,
        forward ? GST_SEARCH_MODE_AFTER : GST_SEARCH_MODE_BEFORE, pos, NULL);
    /* sanity check */
    if (cpos) {
      GST_DEBUG_OBJECT (demux,
          "cluster reported at offset %" G_GINT64_FORMAT, *cpos);
      demux->common.offset = *cpos;
      ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
          GST_ELEMENT_CAST (demux), &id, &length, &needed);
      if (ret == GST_FLOW_OK && id == GST_MATROSKA_ID_CLUSTER) {
        newpos = *cpos;
        goto exit;
      }
    }
  }

  /* read in at newpos and scan for ebml cluster id */
  oldpos = oldlength = -1;
  while (1) {
    GstByteReader reader;
    gint cluster_pos;
    guint toread = chunk;

    if (!forward) {
      /* never read beyond the requested target */
      if (G_UNLIKELY (newpos < chunk)) {
        toread = newpos;
        newpos = 0;
      } else {
        newpos -= chunk;
      }
    }
    if (buf != NULL) {
      gst_buffer_unmap (buf, &map);
      gst_buffer_unref (buf);
      buf = NULL;
    }
    ret = gst_pad_pull_range (demux->common.sinkpad, newpos, toread, &buf);
    if (ret != GST_FLOW_OK)
      break;
    GST_DEBUG_OBJECT (demux,
        "read buffer size %" G_GSIZE_FORMAT " at offset %" G_GINT64_FORMAT,
        gst_buffer_get_size (buf), newpos);
    gst_buffer_map (buf, &map, GST_MAP_READ);
    data = map.data;
    size = map.size;
    if (oldpos == newpos && oldlength == map.size) {
      GST_ERROR_OBJECT (demux, "Stuck at same position");
      ret = GST_FLOW_ERROR;
      goto exit;
    } else {
      oldpos = newpos;
      oldlength = map.size;
    }

    gst_byte_reader_init (&reader, data, size);
    cluster_pos = -1;
    while (1) {
      gint found = gst_byte_reader_masked_scan_uint32 (&reader, 0xffffffff,
          GST_MATROSKA_ID_CLUSTER, 0, gst_byte_reader_get_remaining (&reader));
      if (forward) {
        cluster_pos = found;
        break;
      }
      /* need last occurrence when searching backwards */
      if (found >= 0) {
        cluster_pos = gst_byte_reader_get_pos (&reader) + found;
        gst_byte_reader_skip (&reader, found + 4);
      } else {
        break;
      }
    }

    if (cluster_pos >= 0) {
      newpos += cluster_pos;
      GST_DEBUG_OBJECT (demux,
          "found cluster ebml id at offset %" G_GINT64_FORMAT, newpos);
      /* extra checks whether we really sync'ed to a cluster:
       * - either it is the first and only cluster
       * - either there is a cluster after this one
       * - either cluster length is undefined
       */
      /* ok if first cluster (there may not a subsequent one) */
      if (newpos == demux->first_cluster_offset) {
        GST_DEBUG_OBJECT (demux, "cluster is first cluster -> OK");
        break;
      }
      demux->common.offset = newpos;
      ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
          GST_ELEMENT_CAST (demux), &id, &length, &needed);
      if (ret != GST_FLOW_OK) {
        GST_DEBUG_OBJECT (demux, "need more data -> continue");
        goto next;
      }
      g_assert (id == GST_MATROSKA_ID_CLUSTER);
      GST_DEBUG_OBJECT (demux, "cluster size %" G_GUINT64_FORMAT ", prefix %d",
          length, needed);
      /* ok if undefined length or first cluster */
      if (length == GST_EBML_SIZE_UNKNOWN || length == G_MAXUINT64) {
        GST_DEBUG_OBJECT (demux, "cluster has undefined length -> OK");
        break;
      }
      /* skip cluster */
      demux->common.offset += length + needed;
      ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
          GST_ELEMENT_CAST (demux), &id, &length, &needed);
      if (ret != GST_FLOW_OK)
        goto next;
      GST_DEBUG_OBJECT (demux, "next element is %scluster",
          id == GST_MATROSKA_ID_CLUSTER ? "" : "not ");
      if (id == GST_MATROSKA_ID_CLUSTER)
        break;
    next:
      if (forward)
        newpos += 1;
    } else {
      /* partial cluster id may have been in tail of buffer */
      newpos +=
          forward ? MAX (gst_byte_reader_get_remaining (&reader), 4) - 3 : 3;
    }
  }

  if (buf) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
    buf = NULL;
  }

exit:
  demux->common.offset = orig_offset;
  *pos = newpos;
  return ret;
}