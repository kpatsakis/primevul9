WandExport DrawingWand *AcquireDrawingWand(const DrawInfo *draw_info,
  Image *image)
{
  DrawingWand
    *wand;

  wand=NewDrawingWand();
  if (draw_info != (const DrawInfo *) NULL)
    {
      CurrentContext=DestroyDrawInfo(CurrentContext);
      CurrentContext=CloneDrawInfo((ImageInfo *) NULL,draw_info);
    }
  if (image != (Image *) NULL)
    {
      wand->image=DestroyImage(wand->image);
      wand->destroy=MagickFalse;
    }
  wand->image=image;
  return(wand);
}