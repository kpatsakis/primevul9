gst_matroska_demux_update_tracks (GstMatroskaDemux * demux, GstEbmlRead * ebml)
{
  GstFlowReturn ret = GST_FLOW_OK;
  guint num_tracks_found = 0;
  guint32 id;

  GST_INFO_OBJECT (demux, "Reparsing Tracks element");

  DEBUG_ELEMENT_START (demux, ebml, "Tracks");

  if ((ret = gst_ebml_read_master (ebml, &id)) != GST_FLOW_OK) {
    DEBUG_ELEMENT_STOP (demux, ebml, "Tracks", ret);
    return ret;
  }

  while (ret == GST_FLOW_OK && gst_ebml_read_has_remaining (ebml, 1, TRUE)) {
    if ((ret = gst_ebml_peek_id (ebml, &id)) != GST_FLOW_OK)
      break;

    switch (id) {
        /* one track within the "all-tracks" header */
      case GST_MATROSKA_ID_TRACKENTRY:{
        GstMatroskaTrackContext *new_track;
        gint old_track_index;
        GstMatroskaTrackContext *old_track;
        ret = gst_matroska_demux_parse_stream (demux, ebml, &new_track);
        if (new_track == NULL)
          break;
        num_tracks_found++;

        if (gst_matroska_read_common_tracknumber_unique (&demux->common,
                new_track->num)) {
          GST_ERROR_OBJECT (demux,
              "Unexpected new TrackNumber: %" G_GUINT64_FORMAT, new_track->num);
          goto track_mismatch_error;
        }

        old_track_index =
            gst_matroska_read_common_stream_from_num (&demux->common,
            new_track->num);
        g_assert (old_track_index != -1);
        old_track = g_ptr_array_index (demux->common.src, old_track_index);

        if (old_track->type != new_track->type) {
          GST_ERROR_OBJECT (demux,
              "Mismatch reparsing track %" G_GUINT64_FORMAT
              " on track type. Expected %d, found %d", new_track->num,
              old_track->type, new_track->type);
          goto track_mismatch_error;
        }

        if (g_strcmp0 (old_track->codec_id, new_track->codec_id) != 0) {
          GST_ERROR_OBJECT (demux,
              "Mismatch reparsing track %" G_GUINT64_FORMAT
              " on codec id. Expected '%s', found '%s'", new_track->num,
              old_track->codec_id, new_track->codec_id);
          goto track_mismatch_error;
        }

        /* The new track matches the old track. No problems on our side.
         * Let's make it replace the old track. */
        new_track->pad = old_track->pad;
        new_track->index = old_track->index;
        new_track->pos = old_track->pos;
        g_ptr_array_index (demux->common.src, old_track_index) = new_track;
        gst_pad_set_element_private (new_track->pad, new_track);

        if (!gst_caps_is_equal (old_track->caps, new_track->caps)) {
          gst_pad_set_caps (new_track->pad, new_track->caps);
        }
        gst_caps_replace (&old_track->caps, NULL);

        if (!gst_tag_list_is_equal (old_track->tags, new_track->tags)) {
          GST_DEBUG_OBJECT (old_track->pad, "Sending tags %p: %"
              GST_PTR_FORMAT, new_track->tags, new_track->tags);
          gst_pad_push_event (new_track->pad,
              gst_event_new_tag (gst_tag_list_copy (new_track->tags)));
        }

        gst_matroska_track_free (old_track);
        break;

      track_mismatch_error:
        gst_matroska_track_free (new_track);
        new_track = NULL;
        ret = GST_FLOW_ERROR;
        break;
      }

      default:
        ret = gst_matroska_read_common_parse_skip (&demux->common, ebml,
            "Track", id);
        break;
    }
  }
  DEBUG_ELEMENT_STOP (demux, ebml, "Tracks", ret);

  if (ret != GST_FLOW_ERROR && demux->common.num_streams != num_tracks_found) {
    GST_ERROR_OBJECT (demux,
        "Mismatch on the number of tracks. Expected %du tracks, found %du",
        demux->common.num_streams, num_tracks_found);
    ret = GST_FLOW_ERROR;
  }

  return ret;
}