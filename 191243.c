qtdemux_is_string_tag_3gp (GstQTDemux * qtdemux, guint32 fourcc)
{
  return fourcc == FOURCC_cprt || fourcc == FOURCC_gnre || fourcc == FOURCC_titl
      || fourcc == FOURCC_dscp || fourcc == FOURCC_perf || fourcc == FOURCC_auth
      || fourcc == FOURCC_albm;
}