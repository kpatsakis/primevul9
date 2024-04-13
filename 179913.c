MagickExport MagickBooleanType SetPixelMetaChannels(Image *image,
  const size_t number_meta_channels,ExceptionInfo *exception)
{
  image->number_meta_channels=number_meta_channels;
  InitializePixelChannelMap(image);
  return(SyncImagePixelCache(image,exception));
}