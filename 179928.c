MagickExport void InitializePixelChannelMap(Image *image)
{
  PixelTrait
    trait;

  register ssize_t
    i;

  ssize_t
    n;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) memset(image->channel_map,0,MaxPixelChannels*
    sizeof(*image->channel_map));
  trait=UpdatePixelTrait;
  if (image->alpha_trait != UndefinedPixelTrait)
    trait=(PixelTrait) (trait | BlendPixelTrait);
  n=0;
  if ((image->colorspace == LinearGRAYColorspace) ||
      (image->colorspace == GRAYColorspace))
    {
      SetPixelChannelAttributes(image,BluePixelChannel,trait,n);
      SetPixelChannelAttributes(image,GreenPixelChannel,trait,n);
      SetPixelChannelAttributes(image,RedPixelChannel,trait,n++);
    }
  else
    {
      SetPixelChannelAttributes(image,RedPixelChannel,trait,n++);
      SetPixelChannelAttributes(image,GreenPixelChannel,trait,n++);
      SetPixelChannelAttributes(image,BluePixelChannel,trait,n++);
    }
  if (image->colorspace == CMYKColorspace)
    SetPixelChannelAttributes(image,BlackPixelChannel,trait,n++);
  for (i=0; i < (ssize_t) image->number_meta_channels; i++)
  {
    SetPixelChannelAttributes(image,(PixelChannel) n,UpdatePixelTrait,n);
    n++;
  }
  if (image->alpha_trait != UndefinedPixelTrait)
    SetPixelChannelAttributes(image,AlphaPixelChannel,CopyPixelTrait,n++);
  if (image->storage_class == PseudoClass)
    SetPixelChannelAttributes(image,IndexPixelChannel,CopyPixelTrait,n++);
  if ((image->channels & ReadMaskChannel) != 0)
    SetPixelChannelAttributes(image,ReadMaskPixelChannel,CopyPixelTrait,n++);
  if ((image->channels & WriteMaskChannel) != 0)
    SetPixelChannelAttributes(image,WriteMaskPixelChannel,CopyPixelTrait,n++);
  if ((image->channels & CompositeMaskChannel) != 0)
    SetPixelChannelAttributes(image,CompositeMaskPixelChannel,CopyPixelTrait,
      n++);
  image->number_channels=(size_t) n;
  (void) SetPixelChannelMask(image,image->channel_mask);
}