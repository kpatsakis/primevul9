WandExport void DrawComment(DrawingWand *wand,const char *comment)
{
  (void) MVGPrintf(wand,"#%s\n",comment);
}