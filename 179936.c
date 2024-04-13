MagickExport PixelChannelMap *AcquirePixelChannelMap(void)
{
  PixelChannelMap
    *channel_map;

  register ssize_t
    i;

  channel_map=(PixelChannelMap *) AcquireQuantumMemory(MaxPixelChannels,
    sizeof(*channel_map));
  if (channel_map == (PixelChannelMap *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(channel_map,0,MaxPixelChannels*sizeof(*channel_map));
  for (i=0; i < MaxPixelChannels; i++)
    channel_map[i].channel=(PixelChannel) i;
  return(channel_map);
}