MagickExport void XDestroyResourceInfo(XResourceInfo *resource_info)
{
  if (resource_info->image_geometry != (char *) NULL)
    resource_info->image_geometry=(char *)
      RelinquishMagickMemory(resource_info->image_geometry);
  if (resource_info->quantize_info != (QuantizeInfo *) NULL)
    resource_info->quantize_info=DestroyQuantizeInfo(
      resource_info->quantize_info);
  if (resource_info->client_name != (char *) NULL)
    resource_info->client_name=(char *)
      RelinquishMagickMemory(resource_info->client_name);
  if (resource_info->name != (char *) NULL)
    resource_info->name=DestroyString(resource_info->name);
  (void) memset(resource_info,0,sizeof(*resource_info));
}