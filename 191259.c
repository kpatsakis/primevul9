gst_qtdemux_loop_state_header (GstQTDemux * qtdemux)
{
  guint64 length = 0;
  guint32 fourcc = 0;
  GstBuffer *buf = NULL;
  GstFlowReturn ret = GST_FLOW_OK;
  guint64 cur_offset = qtdemux->offset;
  GstMapInfo map;

  ret = gst_pad_pull_range (qtdemux->sinkpad, cur_offset, 16, &buf);
  if (G_UNLIKELY (ret != GST_FLOW_OK))
    goto beach;
  gst_buffer_map (buf, &map, GST_MAP_READ);
  if (G_LIKELY (map.size >= 8))
    extract_initial_length_and_fourcc (map.data, map.size, &length, &fourcc);
  gst_buffer_unmap (buf, &map);
  gst_buffer_unref (buf);

  /* maybe we already got most we needed, so only consider this eof */
  if (G_UNLIKELY (length == 0)) {
    GST_ELEMENT_WARNING (qtdemux, STREAM, DEMUX,
        (_("Invalid atom size.")),
        ("Header atom '%" GST_FOURCC_FORMAT "' has empty length",
            GST_FOURCC_ARGS (fourcc)));
    ret = GST_FLOW_EOS;
    goto beach;
  }

  switch (fourcc) {
    case FOURCC_moof:
      /* record for later parsing when needed */
      if (!qtdemux->moof_offset) {
        qtdemux->moof_offset = qtdemux->offset;
      }
      if (qtdemux_pull_mfro_mfra (qtdemux)) {
        /* FIXME */
      } else {
        qtdemux->offset += length;      /* skip moof and keep going */
      }
      if (qtdemux->got_moov) {
        GST_INFO_OBJECT (qtdemux, "moof header, got moov, done with headers");
        ret = GST_FLOW_EOS;
        goto beach;
      }
      break;
    case FOURCC_mdat:
    case FOURCC_free:
    case FOURCC_wide:
    case FOURCC_PICT:
    case FOURCC_pnot:
    {
      GST_LOG_OBJECT (qtdemux,
          "skipping atom '%" GST_FOURCC_FORMAT "' at %" G_GUINT64_FORMAT,
          GST_FOURCC_ARGS (fourcc), cur_offset);
      qtdemux->offset = add_offset (qtdemux->offset, length);
      break;
    }
    case FOURCC_moov:
    {
      GstBuffer *moov = NULL;

      if (qtdemux->got_moov) {
        GST_DEBUG_OBJECT (qtdemux, "Skipping moov atom as we have one already");
        qtdemux->offset = add_offset (qtdemux->offset, length);
        goto beach;
      }

      ret = gst_pad_pull_range (qtdemux->sinkpad, cur_offset, length, &moov);
      if (ret != GST_FLOW_OK)
        goto beach;
      gst_buffer_map (moov, &map, GST_MAP_READ);

      if (length != map.size) {
        /* Some files have a 'moov' atom at the end of the file which contains
         * a terminal 'free' atom where the body of the atom is missing.
         * Check for, and permit, this special case.
         */
        if (map.size >= 8) {
          guint8 *final_data = map.data + (map.size - 8);
          guint32 final_length = QT_UINT32 (final_data);
          guint32 final_fourcc = QT_FOURCC (final_data + 4);

          if (final_fourcc == FOURCC_free
              && map.size + final_length - 8 == length) {
            /* Ok, we've found that special case. Allocate a new buffer with
             * that free atom actually present. */
            GstBuffer *newmoov = gst_buffer_new_and_alloc (length);
            gst_buffer_fill (newmoov, 0, map.data, map.size);
            gst_buffer_memset (newmoov, map.size, 0, final_length - 8);
            gst_buffer_unmap (moov, &map);
            gst_buffer_unref (moov);
            moov = newmoov;
            gst_buffer_map (moov, &map, GST_MAP_READ);
          }
        }
      }

      if (length != map.size) {
        GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
            (_("This file is incomplete and cannot be played.")),
            ("We got less than expected (received %" G_GSIZE_FORMAT
                ", wanted %u, offset %" G_GUINT64_FORMAT ")", map.size,
                (guint) length, cur_offset));
        gst_buffer_unmap (moov, &map);
        gst_buffer_unref (moov);
        ret = GST_FLOW_ERROR;
        goto beach;
      }
      qtdemux->offset += length;

      qtdemux_parse_moov (qtdemux, map.data, length);
      qtdemux_node_dump (qtdemux, qtdemux->moov_node);

      qtdemux_parse_tree (qtdemux);
      g_node_destroy (qtdemux->moov_node);
      gst_buffer_unmap (moov, &map);
      gst_buffer_unref (moov);
      qtdemux->moov_node = NULL;
      qtdemux->got_moov = TRUE;

      break;
    }
    case FOURCC_ftyp:
    {
      GstBuffer *ftyp = NULL;

      /* extract major brand; might come in handy for ISO vs QT issues */
      ret = gst_qtdemux_pull_atom (qtdemux, cur_offset, length, &ftyp);
      if (ret != GST_FLOW_OK)
        goto beach;
      qtdemux->offset += length;
      gst_buffer_map (ftyp, &map, GST_MAP_READ);
      qtdemux_parse_ftyp (qtdemux, map.data, map.size);
      gst_buffer_unmap (ftyp, &map);
      gst_buffer_unref (ftyp);
      break;
    }
    case FOURCC_uuid:
    {
      GstBuffer *uuid = NULL;

      /* uuid are extension atoms */
      ret = gst_qtdemux_pull_atom (qtdemux, cur_offset, length, &uuid);
      if (ret != GST_FLOW_OK)
        goto beach;
      qtdemux->offset += length;
      gst_buffer_map (uuid, &map, GST_MAP_READ);
      qtdemux_parse_uuid (qtdemux, map.data, map.size);
      gst_buffer_unmap (uuid, &map);
      gst_buffer_unref (uuid);
      break;
    }
    case FOURCC_sidx:
    {
      GstBuffer *sidx = NULL;
      ret = gst_qtdemux_pull_atom (qtdemux, cur_offset, length, &sidx);
      if (ret != GST_FLOW_OK)
        goto beach;
      qtdemux->offset += length;
      gst_buffer_map (sidx, &map, GST_MAP_READ);
      qtdemux_parse_sidx (qtdemux, map.data, map.size);
      gst_buffer_unmap (sidx, &map);
      gst_buffer_unref (sidx);
      break;
    }
    default:
    {
      GstBuffer *unknown = NULL;

      GST_LOG_OBJECT (qtdemux,
          "unknown %08x '%" GST_FOURCC_FORMAT "' of size %" G_GUINT64_FORMAT
          " at %" G_GUINT64_FORMAT, fourcc, GST_FOURCC_ARGS (fourcc), length,
          cur_offset);
      ret = gst_qtdemux_pull_atom (qtdemux, cur_offset, length, &unknown);
      if (ret != GST_FLOW_OK)
        goto beach;
      gst_buffer_map (unknown, &map, GST_MAP_READ);
      GST_MEMDUMP ("Unknown tag", map.data, map.size);
      gst_buffer_unmap (unknown, &map);
      gst_buffer_unref (unknown);
      qtdemux->offset += length;
      break;
    }
  }

beach:
  if (ret == GST_FLOW_EOS && (qtdemux->got_moov || qtdemux->media_caps)) {
    /* digested all data, show what we have */
    qtdemux_prepare_streams (qtdemux);
    ret = qtdemux_expose_streams (qtdemux);

    qtdemux->state = QTDEMUX_STATE_MOVIE;
    GST_DEBUG_OBJECT (qtdemux, "switching state to STATE_MOVIE (%d)",
        qtdemux->state);
    return ret;
  }
  return ret;
}