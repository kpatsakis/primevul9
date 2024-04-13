gst_aac_parse_get_audio_sampling_frequency_index (gint sample_rate)
{
  switch (sample_rate) {
    case 96000:
      return 0x0U;
    case 88200:
      return 0x1U;
    case 64000:
      return 0x2U;
    case 48000:
      return 0x3U;
    case 44100:
      return 0x4U;
    case 32000:
      return 0x5U;
    case 24000:
      return 0x6U;
    case 22050:
      return 0x7U;
    case 16000:
      return 0x8U;
    case 12000:
      return 0x9U;
    case 11025:
      return 0xAU;
    case 8000:
      return 0xBU;
    case 7350:
      return 0xCU;
    default:
      return G_MAXUINT8;
  }
}