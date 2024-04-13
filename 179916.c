MagickExport ChannelType SetPixelChannelMask(Image *image,
  const ChannelType channel_mask)
{
#define GetChannelBit(mask,bit)  (((size_t) (mask) >> (size_t) (bit)) & 0x01)

  ChannelType
    mask;

  register ssize_t
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(PixelEvent,GetMagickModule(),"%s[%08x]",
      image->filename,channel_mask);
  mask=image->channel_mask;
  image->channel_mask=channel_mask;
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel = GetPixelChannelChannel(image,i);
    if (GetChannelBit(channel_mask,channel) == 0)
      {
        SetPixelChannelTraits(image,channel,CopyPixelTrait);
        continue;
      }
    if (channel == AlphaPixelChannel)
      {
        if ((image->alpha_trait & CopyPixelTrait) != 0)
          {
            SetPixelChannelTraits(image,channel,CopyPixelTrait);
            continue;
          }
        SetPixelChannelTraits(image,channel,UpdatePixelTrait);
        continue;
      }
    if (image->alpha_trait != UndefinedPixelTrait)
      {
        SetPixelChannelTraits(image,channel,(const PixelTrait)
          (UpdatePixelTrait | BlendPixelTrait));
        continue;
      }
    SetPixelChannelTraits(image,channel,UpdatePixelTrait);
  }
  if (image->storage_class == PseudoClass)
    SetPixelChannelTraits(image,IndexPixelChannel,CopyPixelTrait);
  if ((image->channels & ReadMaskChannel) != 0)
    SetPixelChannelTraits(image,ReadMaskPixelChannel,CopyPixelTrait);
  if ((image->channels & WriteMaskChannel) != 0)
    SetPixelChannelTraits(image,WriteMaskPixelChannel,CopyPixelTrait);
  if ((image->channels & CompositeMaskChannel) != 0)
    SetPixelChannelTraits(image,CompositeMaskPixelChannel,CopyPixelTrait);
  if (image->debug != MagickFalse)
    LogPixelChannels(image);
  return(mask);
}