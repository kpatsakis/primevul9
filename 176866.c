gst_matroska_demux_parse_colour (GstMatroskaDemux * demux, GstEbmlRead * ebml,
    GstMatroskaTrackVideoContext * video_context)
{
  GstFlowReturn ret;
  GstVideoColorimetry colorimetry;
  guint32 id;
  guint64 num;

  colorimetry.range = GST_VIDEO_COLOR_RANGE_UNKNOWN;
  colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_UNKNOWN;
  colorimetry.transfer = GST_VIDEO_TRANSFER_UNKNOWN;
  colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_UNKNOWN;

  DEBUG_ELEMENT_START (demux, ebml, "TrackVideoColour");

  if ((ret = gst_ebml_read_master (ebml, &id)) != GST_FLOW_OK)
    goto beach;

  while (ret == GST_FLOW_OK && gst_ebml_read_has_remaining (ebml, 1, TRUE)) {
    if ((ret = gst_ebml_peek_id (ebml, &id)) != GST_FLOW_OK)
      goto beach;

    switch (id) {
      case GST_MATROSKA_ID_VIDEOMATRIXCOEFFICIENTS:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;

        colorimetry.matrix = gst_video_color_matrix_from_iso ((guint) num);
        break;
      }

      case GST_MATROSKA_ID_VIDEORANGE:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;

        switch (num) {
          case 0:
            colorimetry.range = GST_VIDEO_COLOR_RANGE_UNKNOWN;
            break;
          case 1:
            colorimetry.range = GST_VIDEO_COLOR_RANGE_16_235;
            break;
          case 2:
            colorimetry.range = GST_VIDEO_COLOR_RANGE_0_255;
            break;
          default:
            GST_FIXME_OBJECT (demux, "Unsupported color range  %"
                G_GUINT64_FORMAT, num);
            break;
        }
        break;
      }

      case GST_MATROSKA_ID_VIDEOTRANSFERCHARACTERISTICS:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;

        colorimetry.transfer =
            gst_video_transfer_function_from_iso ((guint) num);
        break;
      }

      case GST_MATROSKA_ID_VIDEOPRIMARIES:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;

        colorimetry.primaries =
            gst_video_color_primaries_from_iso ((guint) num);
        break;
      }

      case GST_MATROSKA_ID_MASTERINGMETADATA:{
        if ((ret =
                gst_matroska_demux_parse_mastering_metadata (demux, ebml,
                    video_context)) != GST_FLOW_OK)
          goto beach;
        break;
      }

      case GST_MATROSKA_ID_MAXCLL:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;
        if (num > G_MAXUINT16) {
          GST_WARNING_OBJECT (demux,
              "Too large maxCLL value %" G_GUINT64_FORMAT, num);
        } else {
          video_context->content_light_level.max_content_light_level = num;
        }
        break;
      }

      case GST_MATROSKA_ID_MAXFALL:{
        if ((ret = gst_ebml_read_uint (ebml, &id, &num)) != GST_FLOW_OK)
          goto beach;
        if (num >= G_MAXUINT16) {
          GST_WARNING_OBJECT (demux,
              "Too large maxFALL value %" G_GUINT64_FORMAT, num);
        } else {
          video_context->content_light_level.max_frame_average_light_level =
              num;
        }
        break;
      }

      default:
        GST_FIXME_OBJECT (demux, "Unsupported subelement 0x%x in Colour", id);
        ret = gst_ebml_read_skip (ebml);
        break;
    }
  }

  memcpy (&video_context->colorimetry, &colorimetry,
      sizeof (GstVideoColorimetry));

beach:
  DEBUG_ELEMENT_STOP (demux, ebml, "TrackVideoColour", ret);
  return ret;
}