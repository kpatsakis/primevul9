gst_matroska_demux_parse_tracks (GstMatroskaDemux * demux, GstEbmlRead * ebml)
{
  GstFlowReturn ret = GST_FLOW_OK;
  guint32 id;

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
        GstMatroskaTrackContext *track;
        ret = gst_matroska_demux_parse_stream (demux, ebml, &track);
        if (track != NULL) {
          if (gst_matroska_read_common_tracknumber_unique (&demux->common,
                  track->num)) {
            gst_matroska_demux_add_stream (demux, track);
          } else {
            GST_ERROR_OBJECT (demux,
                "TrackNumber %" G_GUINT64_FORMAT " is not unique", track->num);
            ret = GST_FLOW_ERROR;
            gst_matroska_track_free (track);
            track = NULL;
          }
        }
        break;
      }

      default:
        ret = gst_matroska_read_common_parse_skip (&demux->common, ebml,
            "Track", id);
        break;
    }
  }
  DEBUG_ELEMENT_STOP (demux, ebml, "Tracks", ret);

  demux->tracks_parsed = TRUE;
  GST_DEBUG_OBJECT (demux, "signaling no more pads");
  gst_element_no_more_pads (GST_ELEMENT (demux));

  return ret;
}