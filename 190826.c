gst_aac_parse_get_audio_channel_configuration (gint num_channels)
{
  if (num_channels >= 1 && num_channels <= 6)   /* Mono up to & including 5.1 */
    return (guint8) num_channels;
  else if (num_channels == 8)   /* 7.1 */
    return (guint8) 7U;
  else
    return G_MAXUINT8;

  /* FIXME: Add support for configurations 11, 12 and 14 from
   * ISO/IEC 14496-3:2009/PDAM 4 based on the actual channel layout
   */
}