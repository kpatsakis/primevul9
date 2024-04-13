MagickExport void XGetImportInfo(XImportInfo *ximage_info)
{
  assert(ximage_info != (XImportInfo *) NULL);
  ximage_info->frame=MagickFalse;
  ximage_info->borders=MagickFalse;
  ximage_info->screen=MagickFalse;
  ximage_info->descend=MagickTrue;
  ximage_info->silent=MagickFalse;
}