gst_matroska_demux_plugin_init (GstPlugin * plugin)
{
  gst_riff_init ();

  /* parser helper separate debug */
  GST_DEBUG_CATEGORY_INIT (ebmlread_debug, "ebmlread",
      0, "EBML stream helper class");

  /* create an elementfactory for the matroska_demux element */
  if (!gst_element_register (plugin, "matroskademux",
          GST_RANK_PRIMARY, GST_TYPE_MATROSKA_DEMUX))
    return FALSE;

  return TRUE;
}