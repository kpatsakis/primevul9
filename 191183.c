qtdemux_gst_structure_free (GstStructure * gststructure)
{
  if (gststructure) {
    gst_structure_free (gststructure);
  }
}