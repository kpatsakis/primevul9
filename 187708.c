static Image *RenderSVGImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    background[MagickPathExtent],
    command[MagickPathExtent],
    *density,
    input_filename[MagickPathExtent],
    opacity[MagickPathExtent],
    output_filename[MagickPathExtent],
    unique[MagickPathExtent];

  const DelegateInfo
    *delegate_info;

  Image
    *next;

  int
    status;

  struct stat
    attributes;

  /*
    Our best hope for compliance with the SVG standard.
  */
  delegate_info=GetDelegateInfo("svg:decode",(char *) NULL,exception);
  if (delegate_info == (const DelegateInfo *) NULL)
    return((Image *) NULL);
  status=AcquireUniqueSymbolicLink(image->filename,input_filename);
  (void) AcquireUniqueFilename(output_filename);
  (void) AcquireUniqueFilename(unique);
  density=AcquireString("");
  (void) FormatLocaleString(density,MagickPathExtent,"%.20g,%.20g",
    image->x_resolution,image->y_resolution);
  (void) FormatLocaleString(background,MagickPathExtent,
    "rgb(%.20g%%,%.20g%%,%.20g%%)",
    100.0*QuantumScale*image->background_color.red,
    100.0*QuantumScale*image->background_color.green,
    100.0*QuantumScale*image->background_color.blue);
  (void) FormatLocaleString(opacity,MagickPathExtent,"%.20g",QuantumScale*
    (QuantumRange-image->background_color.opacity));
  (void) FormatLocaleString(command,MagickPathExtent,
    GetDelegateCommands(delegate_info),input_filename,output_filename,density,
    background,opacity,unique);
  density=DestroyString(density);
  status=ExternalDelegateCommand(MagickFalse,image_info->verbose,command,
    (char *) NULL,exception);
  (void) RelinquishUniqueFileResource(unique);
  (void) RelinquishUniqueFileResource(input_filename);
  if ((status == 0) && (stat(output_filename,&attributes) == 0) &&
      (attributes.st_size > 0))
    {
      Image
        *svg_image;

      ImageInfo
        *read_info;

      read_info=CloneImageInfo(image_info);
      (void) CopyMagickString(read_info->filename,output_filename,
        MagickPathExtent);
      svg_image=ReadImage(read_info,exception);
      read_info=DestroyImageInfo(read_info);
      if (svg_image != (Image *) NULL)
        {
          (void) RelinquishUniqueFileResource(output_filename);
          for (next=GetFirstImageInList(svg_image); next != (Image *) NULL; )
          {
            (void) CopyMagickString(next->filename,image->filename,
              MaxTextExtent);
            (void) CopyMagickString(next->magick,image->magick,MaxTextExtent);
            next=GetNextImageInList(next);
          }
          return(svg_image);
        }
    }
  (void) RelinquishUniqueFileResource(output_filename);
  return((Image *) NULL);
}