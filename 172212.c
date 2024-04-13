void JPXStream::getImageParams(int *bitsPerComponent, StreamColorSpaceMode *csMode) {
  if (unlikely(priv->inited == false)) { init(); }

  *bitsPerComponent = 8;
  int numComps = (priv->image) ? priv->image->numcomps : 1;
  if (priv->image) {
    if (priv->image->color_space == OPJ_CLRSPC_SRGB && numComps == 4) { numComps = 3; }
    else if (priv->image->color_space == OPJ_CLRSPC_SYCC && numComps == 4) { numComps = 3; }
    else if (numComps == 2) { numComps = 1; }
    else if (numComps > 4) { numComps = 4; }
  }
  if (numComps == 3)
    *csMode = streamCSDeviceRGB;
  else if (numComps == 4)
    *csMode = streamCSDeviceCMYK;
  else
    *csMode = streamCSDeviceGray;
}