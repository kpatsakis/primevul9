gst_qtdemux_post_no_playable_stream_error (GstQTDemux * qtdemux)
{
  if (qtdemux->posted_redirect) {
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file contains no playable streams.")),
        ("no known streams found, a redirect message has been posted"));
  } else {
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file contains no playable streams.")),
        ("no known streams found"));
  }
}