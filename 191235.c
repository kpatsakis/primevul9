qtdemux_parse_moof (GstQTDemux * qtdemux, const guint8 * buffer, guint length,
    guint64 moof_offset, QtDemuxStream * stream)
{
  GNode *moof_node, *traf_node, *tfhd_node, *trun_node, *tfdt_node, *mfhd_node;
  GNode *uuid_node;
  GstByteReader mfhd_data, trun_data, tfhd_data, tfdt_data;
  GNode *saiz_node, *saio_node, *pssh_node;
  GstByteReader saiz_data, saio_data;
  guint32 ds_size = 0, ds_duration = 0, ds_flags = 0;
  gint64 base_offset, running_offset;
  guint32 frag_num;

  /* NOTE @stream ignored */

  moof_node = g_node_new ((guint8 *) buffer);
  qtdemux_parse_node (qtdemux, moof_node, buffer, length);
  qtdemux_node_dump (qtdemux, moof_node);

  /* Get fragment number from mfhd and check it's valid */
  mfhd_node =
      qtdemux_tree_get_child_by_type_full (moof_node, FOURCC_mfhd, &mfhd_data);
  if (mfhd_node == NULL)
    goto missing_mfhd;
  if (!qtdemux_parse_mfhd (qtdemux, &mfhd_data, &frag_num))
    goto fail;
  GST_DEBUG_OBJECT (qtdemux, "Fragment #%d", frag_num);

  /* unknown base_offset to start with */
  base_offset = running_offset = -1;
  traf_node = qtdemux_tree_get_child_by_type (moof_node, FOURCC_traf);
  while (traf_node) {
    guint64 decode_time = 0;

    /* Fragment Header node */
    tfhd_node =
        qtdemux_tree_get_child_by_type_full (traf_node, FOURCC_tfhd,
        &tfhd_data);
    if (!tfhd_node)
      goto missing_tfhd;
    if (!qtdemux_parse_tfhd (qtdemux, &tfhd_data, &stream, &ds_duration,
            &ds_size, &ds_flags, &base_offset))
      goto missing_tfhd;

    /* The following code assumes at most a single set of sample auxiliary
     * data in the fragment (consisting of a saiz box and a corresponding saio
     * box); in theory, however, there could be multiple sets of sample
     * auxiliary data in a fragment. */
    saiz_node =
        qtdemux_tree_get_child_by_type_full (traf_node, FOURCC_saiz,
        &saiz_data);
    if (saiz_node) {
      guint32 info_type = 0;
      guint64 offset = 0;
      guint32 info_type_parameter = 0;

      g_free (qtdemux->cenc_aux_info_sizes);

      qtdemux->cenc_aux_info_sizes =
          qtdemux_parse_saiz (qtdemux, stream, &saiz_data,
          &qtdemux->cenc_aux_sample_count);
      if (qtdemux->cenc_aux_info_sizes == NULL) {
        GST_ERROR_OBJECT (qtdemux, "failed to parse saiz box");
        goto fail;
      }
      saio_node =
          qtdemux_tree_get_child_by_type_full (traf_node, FOURCC_saio,
          &saio_data);
      if (!saio_node) {
        GST_ERROR_OBJECT (qtdemux, "saiz box without a corresponding saio box");
        g_free (qtdemux->cenc_aux_info_sizes);
        qtdemux->cenc_aux_info_sizes = NULL;
        goto fail;
      }

      if (G_UNLIKELY (!qtdemux_parse_saio (qtdemux, stream, &saio_data,
                  &info_type, &info_type_parameter, &offset))) {
        GST_ERROR_OBJECT (qtdemux, "failed to parse saio box");
        g_free (qtdemux->cenc_aux_info_sizes);
        qtdemux->cenc_aux_info_sizes = NULL;
        goto fail;
      }
      if (base_offset > -1 && base_offset > qtdemux->moof_offset)
        offset += (guint64) (base_offset - qtdemux->moof_offset);
      if (info_type == FOURCC_cenc && info_type_parameter == 0U) {
        GstByteReader br;
        if (offset > length) {
          GST_DEBUG_OBJECT (qtdemux, "cenc auxiliary info stored out of moof");
          qtdemux->cenc_aux_info_offset = offset;
        } else {
          gst_byte_reader_init (&br, buffer + offset, length - offset);
          if (!qtdemux_parse_cenc_aux_info (qtdemux, stream, &br,
                  qtdemux->cenc_aux_info_sizes,
                  qtdemux->cenc_aux_sample_count)) {
            GST_ERROR_OBJECT (qtdemux, "failed to parse cenc auxiliary info");
            g_free (qtdemux->cenc_aux_info_sizes);
            qtdemux->cenc_aux_info_sizes = NULL;
            goto fail;
          }
        }
      }
    }

    tfdt_node =
        qtdemux_tree_get_child_by_type_full (traf_node, FOURCC_tfdt,
        &tfdt_data);
    if (tfdt_node) {
      /* We'll use decode_time to interpolate timestamps
       * in case the input timestamps are missing */
      qtdemux_parse_tfdt (qtdemux, &tfdt_data, &decode_time);

      GST_DEBUG_OBJECT (qtdemux, "decode time %" G_GINT64_FORMAT
          " (%" GST_TIME_FORMAT ")", decode_time,
          GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream, decode_time)));

      /* Discard the fragment buffer timestamp info to avoid using it.
       * Rely on tfdt instead as it is more accurate than the timestamp
       * that is fetched from a manifest/playlist and is usually
       * less accurate. */
      qtdemux->fragment_start = -1;
    }

    if (G_UNLIKELY (!stream)) {
      /* we lost track of offset, we'll need to regain it,
       * but can delay complaining until later or avoid doing so altogether */
      base_offset = -2;
      goto next;
    }
    if (G_UNLIKELY (base_offset < -1))
      goto lost_offset;

    if (qtdemux->upstream_format_is_time)
      gst_qtdemux_stream_flush_samples_data (qtdemux, stream);

    /* initialise moof sample data */
    stream->n_samples_moof = 0;
    stream->duration_last_moof = stream->duration_moof;
    stream->duration_moof = 0;

    /* Track Run node */
    trun_node =
        qtdemux_tree_get_child_by_type_full (traf_node, FOURCC_trun,
        &trun_data);
    while (trun_node) {
      qtdemux_parse_trun (qtdemux, &trun_data, stream,
          ds_duration, ds_size, ds_flags, moof_offset, length, &base_offset,
          &running_offset, decode_time);
      /* iterate all siblings */
      trun_node = qtdemux_tree_get_sibling_by_type_full (trun_node, FOURCC_trun,
          &trun_data);
    }

    uuid_node = qtdemux_tree_get_child_by_type (traf_node, FOURCC_uuid);
    if (uuid_node) {
      guint8 *uuid_buffer = (guint8 *) uuid_node->data;
      guint32 box_length = QT_UINT32 (uuid_buffer);

      qtdemux_parse_uuid (qtdemux, uuid_buffer, box_length);
    }

    /* if no new base_offset provided for next traf,
     * base is end of current traf */
    base_offset = running_offset;
    running_offset = -1;

    if (stream->n_samples_moof && stream->duration_moof)
      stream->new_caps = TRUE;

  next:
    /* iterate all siblings */
    traf_node = qtdemux_tree_get_sibling_by_type (traf_node, FOURCC_traf);
  }

  /* parse any protection system info */
  pssh_node = qtdemux_tree_get_child_by_type (moof_node, FOURCC_pssh);
  while (pssh_node) {
    GST_LOG_OBJECT (qtdemux, "Parsing pssh box.");
    qtdemux_parse_pssh (qtdemux, pssh_node);
    pssh_node = qtdemux_tree_get_sibling_by_type (pssh_node, FOURCC_pssh);
  }

  g_node_destroy (moof_node);
  return TRUE;

missing_tfhd:
  {
    GST_DEBUG_OBJECT (qtdemux, "missing tfhd box");
    goto fail;
  }
missing_mfhd:
  {
    GST_DEBUG_OBJECT (qtdemux, "Missing mfhd box");
    goto fail;
  }
lost_offset:
  {
    GST_DEBUG_OBJECT (qtdemux, "lost offset");
    goto fail;
  }
fail:
  {
    g_node_destroy (moof_node);
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file is corrupt and cannot be played.")), (NULL));
    return FALSE;
  }
}