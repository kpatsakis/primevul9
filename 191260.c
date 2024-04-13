gst_qtdemux_clip_buffer (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstBuffer * buf)
{
  guint64 start, stop, cstart, cstop, diff;
  GstClockTime pts, duration;
  gsize size, osize;
  gint num_rate, denom_rate;
  gint frame_size;
  gboolean clip_data;
  guint offset;

  osize = size = gst_buffer_get_size (buf);
  offset = 0;

  /* depending on the type, setup the clip parameters */
  if (stream->subtype == FOURCC_soun) {
    frame_size = stream->bytes_per_frame;
    num_rate = GST_SECOND;
    denom_rate = (gint) stream->rate;
    clip_data = TRUE;
  } else if (stream->subtype == FOURCC_vide) {
    frame_size = size;
    num_rate = stream->fps_n;
    denom_rate = stream->fps_d;
    clip_data = FALSE;
  } else
    goto wrong_type;

  if (frame_size <= 0)
    goto bad_frame_size;

  /* we can only clip if we have a valid pts */
  pts = GST_BUFFER_PTS (buf);
  if (G_UNLIKELY (!GST_CLOCK_TIME_IS_VALID (pts)))
    goto no_pts;

  duration = GST_BUFFER_DURATION (buf);

  if (G_UNLIKELY (!GST_CLOCK_TIME_IS_VALID (duration))) {
    duration =
        gst_util_uint64_scale_int (size / frame_size, num_rate, denom_rate);
  }

  start = pts;
  stop = start + duration;

  if (G_UNLIKELY (!gst_segment_clip (&stream->segment,
              GST_FORMAT_TIME, start, stop, &cstart, &cstop)))
    goto clipped;

  /* see if some clipping happened */
  diff = cstart - start;
  if (diff > 0) {
    pts += diff;
    duration -= diff;

    if (clip_data) {
      /* bring clipped time to samples and to bytes */
      diff = gst_util_uint64_scale_int (diff, denom_rate, num_rate);
      diff *= frame_size;

      GST_DEBUG_OBJECT (qtdemux,
          "clipping start to %" GST_TIME_FORMAT " %"
          G_GUINT64_FORMAT " bytes", GST_TIME_ARGS (cstart), diff);

      offset = diff;
      size -= diff;
    }
  }
  diff = stop - cstop;
  if (diff > 0) {
    duration -= diff;

    if (clip_data) {
      /* bring clipped time to samples and then to bytes */
      diff = gst_util_uint64_scale_int (diff, denom_rate, num_rate);
      diff *= frame_size;
      GST_DEBUG_OBJECT (qtdemux,
          "clipping stop to %" GST_TIME_FORMAT " %" G_GUINT64_FORMAT
          " bytes", GST_TIME_ARGS (cstop), diff);
      size -= diff;
    }
  }

  if (offset != 0 || size != osize)
    gst_buffer_resize (buf, offset, size);

  GST_BUFFER_DTS (buf) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_PTS (buf) = pts;
  GST_BUFFER_DURATION (buf) = duration;

  return buf;

  /* dropped buffer */
wrong_type:
  {
    GST_DEBUG_OBJECT (qtdemux, "unknown stream type");
    return buf;
  }
bad_frame_size:
  {
    GST_DEBUG_OBJECT (qtdemux, "bad frame size");
    return buf;
  }
no_pts:
  {
    GST_DEBUG_OBJECT (qtdemux, "no pts on buffer");
    return buf;
  }
clipped:
  {
    GST_DEBUG_OBJECT (qtdemux, "clipped buffer");
    gst_buffer_unref (buf);
    return NULL;
  }
}