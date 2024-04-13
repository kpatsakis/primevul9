gst_qtdemux_drop_data (GstQTDemux * demux, gint bytes)
{
  g_return_if_fail (bytes <= demux->todrop);

  GST_LOG_OBJECT (demux, "Dropping %d bytes", bytes);
  gst_adapter_flush (demux->adapter, bytes);
  demux->neededbytes -= bytes;
  demux->offset += bytes;
  demux->todrop -= bytes;
}