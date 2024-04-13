qtdemux_get_cenc_sample_properties (GstQTDemux * qtdemux,
    QtDemuxStream * stream, guint sample_index)
{
  QtDemuxCencSampleSetInfo *info = NULL;

  g_return_val_if_fail (stream != NULL, NULL);
  g_return_val_if_fail (stream->protected, NULL);
  g_return_val_if_fail (stream->protection_scheme_info != NULL, NULL);

  info = (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;

  /* Currently, cenc properties for groups of samples are not supported, so
   * simply return a copy of the default sample properties */
  return gst_structure_copy (info->default_properties);
}