MagickExport PixelChannelMap *ClonePixelChannelMap(PixelChannelMap *channel_map)
{
  PixelChannelMap
    *clone_map;

  assert(channel_map != (PixelChannelMap *) NULL);
  clone_map=AcquirePixelChannelMap();
  if (clone_map == (PixelChannelMap *) NULL)
    return((PixelChannelMap *) NULL);
  (void) memcpy(clone_map,channel_map,MaxPixelChannels*
    sizeof(*channel_map));
  return(clone_map);
}