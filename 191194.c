find_func (QtDemuxSample * s1, gint64 * media_time, gpointer user_data)
{
  if ((gint64) s1->timestamp + s1->pts_offset > *media_time)
    return 1;
  if ((gint64) s1->timestamp + s1->pts_offset == *media_time)
    return 0;

  return -1;
}