static void LogPixelChannels(const Image *image)
{
  register ssize_t
    i;

  (void) LogMagickEvent(PixelEvent,GetMagickModule(),"%s[%08x]",
    image->filename,image->channel_mask);
  for (i=0; i < (ssize_t) image->number_channels; i++)
  {
    char
      channel_name[MagickPathExtent],
      traits[MagickPathExtent];

    const char
      *name;

    PixelChannel
      channel;

    channel=GetPixelChannelChannel(image,i);
    switch (channel)
    {
      case RedPixelChannel:
      {
        name="red";
        if (image->colorspace == CMYKColorspace)
          name="cyan";
        if ((image->colorspace == LinearGRAYColorspace) ||
            (image->colorspace == GRAYColorspace))
          name="gray";
        break;
      }
      case GreenPixelChannel:
      {
        name="green";
        if (image->colorspace == CMYKColorspace)
          name="magenta";
        break;
      }
      case BluePixelChannel:
      {
        name="blue";
        if (image->colorspace == CMYKColorspace)
          name="yellow";
        break;
      }
      case BlackPixelChannel:
      {
        name="black";
        if (image->storage_class == PseudoClass)
          name="index";
        break;
      }
      case IndexPixelChannel:
      {
        name="index";
        break;
      }
      case AlphaPixelChannel:
      {
        name="alpha";
        break;
      }
      case ReadMaskPixelChannel:
      {
        name="read-mask";
        break;
      }
      case WriteMaskPixelChannel:
      {
        name="write-mask";
        break;
      }
      case CompositeMaskPixelChannel:
      {
        name="composite-mask";
        break;
      }
      case MetaPixelChannel:
      {
        name="meta";
        break;
      }
      default:
        name="undefined";
    }
    if (image->colorspace ==  UndefinedColorspace)
      {
        (void) FormatLocaleString(channel_name,MagickPathExtent,"%.20g",
          (double) channel);
        name=(const char *) channel_name;
      }
    *traits='\0';
    if ((GetPixelChannelTraits(image,channel) & UpdatePixelTrait) != 0)
      (void) ConcatenateMagickString(traits,"update,",MagickPathExtent);
    if ((GetPixelChannelTraits(image,channel) & BlendPixelTrait) != 0)
      (void) ConcatenateMagickString(traits,"blend,",MagickPathExtent);
    if ((GetPixelChannelTraits(image,channel) & CopyPixelTrait) != 0)
      (void) ConcatenateMagickString(traits,"copy,",MagickPathExtent);
    if (*traits == '\0')
      (void) ConcatenateMagickString(traits,"undefined,",MagickPathExtent);
    traits[strlen(traits)-1]='\0';
    (void) LogMagickEvent(PixelEvent,GetMagickModule(),"  %.20g: %s (%s)",
      (double) i,name,traits);
  }
}