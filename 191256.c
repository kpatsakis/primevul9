gst_qtdemux_stbl_free (QtDemuxStream * stream)
{
  g_free ((gpointer) stream->stco.data);
  stream->stco.data = NULL;
  g_free ((gpointer) stream->stsz.data);
  stream->stsz.data = NULL;
  g_free ((gpointer) stream->stsc.data);
  stream->stsc.data = NULL;
  g_free ((gpointer) stream->stts.data);
  stream->stts.data = NULL;
  g_free ((gpointer) stream->stss.data);
  stream->stss.data = NULL;
  g_free ((gpointer) stream->stps.data);
  stream->stps.data = NULL;
  g_free ((gpointer) stream->ctts.data);
  stream->ctts.data = NULL;
}