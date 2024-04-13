gst_matroska_demux_parse_mastering_metadata (GstMatroskaDemux * demux,
    GstEbmlRead * ebml, GstMatroskaTrackVideoContext * video_context)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstVideoMasteringDisplayInfo minfo;
  guint32 id;
  gdouble num;
  /* Precision defined by HEVC specification */
  const guint chroma_scale = 50000;
  const guint luma_scale = 10000;

  gst_video_mastering_display_info_init (&minfo);

  DEBUG_ELEMENT_START (demux, ebml, "MasteringMetadata");

  if ((ret = gst_ebml_read_master (ebml, &id)) != GST_FLOW_OK)
    goto beach;

  while (ret == GST_FLOW_OK && gst_ebml_read_has_remaining (ebml, 1, TRUE)) {
    if ((ret = gst_ebml_peek_id (ebml, &id)) != GST_FLOW_OK)
      goto beach;

    /* all sub elements have float type */
    if ((ret = gst_ebml_read_float (ebml, &id, &num)) != GST_FLOW_OK)
      goto beach;

    /* chromaticity should be in [0, 1] range */
    if (id >= GST_MATROSKA_ID_PRIMARYRCHROMATICITYX &&
        id <= GST_MATROSKA_ID_WHITEPOINTCHROMATICITYY) {
      if (num < 0 || num > 1.0) {
        GST_WARNING_OBJECT (demux, "0x%x has invalid value %f", id, num);
        goto beach;
      }
    } else if (id == GST_MATROSKA_ID_LUMINANCEMAX ||
        id == GST_MATROSKA_ID_LUMINANCEMIN) {
      /* Note: webM spec said valid range is [0, 999.9999] but
       * 1000 cd/m^2 is generally used value on HDR. Just check guint range here.
       * See https://www.webmproject.org/docs/container/#LuminanceMax
       */
      if (num < 0 || num > (gdouble) (G_MAXUINT32 / luma_scale)) {
        GST_WARNING_OBJECT (demux, "0x%x has invalid value %f", id, num);
        goto beach;
      }
    }

    switch (id) {
      case GST_MATROSKA_ID_PRIMARYRCHROMATICITYX:
        minfo.display_primaries[0].x = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_PRIMARYRCHROMATICITYY:
        minfo.display_primaries[0].y = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_PRIMARYGCHROMATICITYX:
        minfo.display_primaries[1].x = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_PRIMARYGCHROMATICITYY:
        minfo.display_primaries[1].y = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_PRIMARYBCHROMATICITYX:
        minfo.display_primaries[2].x = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_PRIMARYBCHROMATICITYY:
        minfo.display_primaries[2].y = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_WHITEPOINTCHROMATICITYX:
        minfo.white_point.x = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_WHITEPOINTCHROMATICITYY:
        minfo.white_point.y = (guint16) (num * chroma_scale);
        break;
      case GST_MATROSKA_ID_LUMINANCEMAX:
        minfo.max_display_mastering_luminance = (guint32) (num * luma_scale);
        break;
      case GST_MATROSKA_ID_LUMINANCEMIN:
        minfo.min_display_mastering_luminance = (guint32) (num * luma_scale);
        break;
      default:
        GST_FIXME_OBJECT (demux,
            "Unsupported subelement 0x%x in MasteringMetadata", id);
        ret = gst_ebml_read_skip (ebml);
        break;
    }
  }

  video_context->mastering_display_info = minfo;
  video_context->mastering_display_info_present = TRUE;

beach:
  DEBUG_ELEMENT_STOP (demux, ebml, "MasteringMetadata", ret);

  return ret;
}