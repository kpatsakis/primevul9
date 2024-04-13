MagickExport PixelChannelMap *DestroyPixelChannelMap(
  PixelChannelMap *channel_map)
{
  assert(channel_map != (PixelChannelMap *) NULL);
  channel_map=(PixelChannelMap *) RelinquishMagickMemory(channel_map);
  return((PixelChannelMap *) RelinquishMagickMemory(channel_map));
}