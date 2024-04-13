gst_matroska_demux_seek_block (GstMatroskaDemux * demux)
{
  if (G_UNLIKELY (demux->seek_block)) {
    if (!(--demux->seek_block)) {
      return TRUE;
    } else {
      GST_LOG_OBJECT (demux, "should skip block due to seek");
      return FALSE;
    }
  } else {
    return TRUE;
  }
}