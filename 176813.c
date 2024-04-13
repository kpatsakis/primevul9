gst_matroska_cluster_compare (gint64 * i1, gint64 * i2)
{
  if (*i1 < *i2)
    return -1;
  else if (*i1 > *i2)
    return 1;
  else
    return 0;
}