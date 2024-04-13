gst_qtdemux_src_convert (GstQTDemux * qtdemux, GstPad * pad,
    GstFormat src_format, gint64 src_value, GstFormat dest_format,
    gint64 * dest_value)
{
  gboolean res = TRUE;
  QtDemuxStream *stream = gst_pad_get_element_private (pad);
  gint32 index;

  if (stream->subtype != FOURCC_vide) {
    res = FALSE;
    goto done;
  }

  switch (src_format) {
    case GST_FORMAT_TIME:
      switch (dest_format) {
        case GST_FORMAT_BYTES:{
          index = gst_qtdemux_find_index_linear (qtdemux, stream, src_value);
          if (-1 == index) {
            res = FALSE;
            goto done;
          }

          *dest_value = stream->samples[index].offset;

          GST_DEBUG_OBJECT (qtdemux, "Format Conversion Time->Offset :%"
              GST_TIME_FORMAT "->%" G_GUINT64_FORMAT,
              GST_TIME_ARGS (src_value), *dest_value);
          break;
        }
        default:
          res = FALSE;
          break;
      }
      break;
    case GST_FORMAT_BYTES:
      switch (dest_format) {
        case GST_FORMAT_TIME:{
          index =
              gst_qtdemux_find_index_for_given_media_offset_linear (qtdemux,
              stream, src_value);

          if (-1 == index) {
            res = FALSE;
            goto done;
          }

          *dest_value =
              QTSTREAMTIME_TO_GSTTIME (stream,
              stream->samples[index].timestamp);
          GST_DEBUG_OBJECT (qtdemux,
              "Format Conversion Offset->Time :%" G_GUINT64_FORMAT "->%"
              GST_TIME_FORMAT, src_value, GST_TIME_ARGS (*dest_value));
          break;
        }
        default:
          res = FALSE;
          break;
      }
      break;
    default:
      res = FALSE;
      break;
  }

done:
  return res;
}