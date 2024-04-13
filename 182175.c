MagickExport void XDisplayImageInfo(Display *display,
  const XResourceInfo *resource_info,XWindows *windows,Image *undo_image,
  Image *image)
{
  char
    filename[MaxTextExtent],
    *text,
    **textlist;

  FILE
    *file;

  int
    unique_file;

  register ssize_t
    i;

  size_t
    number_pixels;

  ssize_t
    bytes;

  unsigned int
    levels;

  /*
    Write info about the X server to a file.
  */
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(windows != (XWindows *) NULL);
  assert(image != (Image *) NULL);
  if (image->debug)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"w");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      XNoticeWidget(display,windows,"Unable to display image info",filename);
      return;
    }
  if (resource_info->gamma_correct != MagickFalse)
    if (resource_info->display_gamma != (char *) NULL)
      (void) FormatLocaleFile(file,"Display\n  gamma: %s\n\n",
        resource_info->display_gamma);
  /*
    Write info about the X image to a file.
  */
  (void) FormatLocaleFile(file,"X\n  visual: %s\n",
    XVisualClassName((int) windows->image.storage_class));
  (void) FormatLocaleFile(file,"  depth: %d\n",windows->image.ximage->depth);
  if (windows->visual_info->colormap_size != 0)
    (void) FormatLocaleFile(file,"  colormap size: %d\n",
      windows->visual_info->colormap_size);
  if (resource_info->colormap== SharedColormap)
    (void) FormatLocaleFile(file,"  colormap type: Shared\n");
  else
    (void) FormatLocaleFile(file,"  colormap type: Private\n");
  (void) FormatLocaleFile(file,"  geometry: %dx%d\n",
    windows->image.ximage->width,windows->image.ximage->height);
  if (windows->image.crop_geometry != (char *) NULL)
    (void) FormatLocaleFile(file,"  crop geometry: %s\n",
      windows->image.crop_geometry);
  if (windows->image.pixmap == (Pixmap) NULL)
    (void) FormatLocaleFile(file,"  type: X Image\n");
  else
    (void) FormatLocaleFile(file,"  type: Pixmap\n");
  if (windows->image.shape != MagickFalse)
    (void) FormatLocaleFile(file,"  non-rectangular shape: True\n");
  else
    (void) FormatLocaleFile(file,"  non-rectangular shape: False\n");
  if (windows->image.shared_memory != MagickFalse)
    (void) FormatLocaleFile(file,"  shared memory: True\n");
  else
    (void) FormatLocaleFile(file,"  shared memory: False\n");
  (void) FormatLocaleFile(file,"\n");
  if (resource_info->font != (char *) NULL)
    (void) FormatLocaleFile(file,"Font: %s\n\n",resource_info->font);
  if (resource_info->text_font != (char *) NULL)
    (void) FormatLocaleFile(file,"Text font: %s\n\n",resource_info->text_font);
  /*
    Write info about the undo cache to a file.
  */
  bytes=0;
  for (levels=0; undo_image != (Image *) NULL; levels++)
  {
    number_pixels=undo_image->list->columns*undo_image->list->rows;
    bytes+=number_pixels*sizeof(PixelPacket);
    undo_image=GetPreviousImageInList(undo_image);
  }
  (void) FormatLocaleFile(file,"Undo Edit Cache\n  levels: %u\n",levels);
  (void) FormatLocaleFile(file,"  bytes: %.20gmb\n",(double)
    ((bytes+(1 << 19)) >> 20));
  (void) FormatLocaleFile(file,"  limit: %.20gmb\n\n",(double)
    resource_info->undo_cache);
  /*
    Write info about the image to a file.
  */
  (void) IdentifyImage(image,file,MagickTrue);
  (void) fclose(file);
  text=FileToString(filename,~0UL,&image->exception);
  (void) RelinquishUniqueFileResource(filename);
  if (text == (char *) NULL)
    {
      XNoticeWidget(display,windows,"MemoryAllocationFailed",
        "UnableToDisplayImageInfo");
      return;
    }
  textlist=StringToList(text);
  if (textlist != (char **) NULL)
    {
      char
        title[MaxTextExtent];

      /*
        Display information about the image in the Text View widget.
      */
      (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      (void) FormatLocaleString(title,MaxTextExtent,"Image Info: %s",
        image->filename);
      XTextViewWidget(display,resource_info,windows,MagickTrue,title,
        (char const **) textlist);
      for (i=0; textlist[i] != (char *) NULL; i++)
        textlist[i]=DestroyString(textlist[i]);
      textlist=(char **) RelinquishMagickMemory(textlist);
    }
  text=DestroyString(text);
}