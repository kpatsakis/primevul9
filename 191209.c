gst_qtdemux_combine_flows (GstQTDemux * demux, QtDemuxStream * stream,
    GstFlowReturn ret)
{
  GST_LOG_OBJECT (demux, "flow return: %s", gst_flow_get_name (ret));

  if (stream->pad)
    ret = gst_flow_combiner_update_pad_flow (demux->flowcombiner, stream->pad,
        ret);
  else
    ret = gst_flow_combiner_update_flow (demux->flowcombiner, ret);

  GST_LOG_OBJECT (demux, "combined flow return: %s", gst_flow_get_name (ret));
  return ret;
}