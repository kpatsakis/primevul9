gst_matroska_demux_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GstMatroskaDemux *demux;

  g_return_if_fail (GST_IS_MATROSKA_DEMUX (object));
  demux = GST_MATROSKA_DEMUX (object);

  switch (prop_id) {
    case PROP_MAX_GAP_TIME:
      GST_OBJECT_LOCK (demux);
      demux->max_gap_time = g_value_get_uint64 (value);
      GST_OBJECT_UNLOCK (demux);
      break;
    case PROP_MAX_BACKTRACK_DISTANCE:
      GST_OBJECT_LOCK (demux);
      demux->max_backtrack_distance = g_value_get_uint (value);
      GST_OBJECT_UNLOCK (demux);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}