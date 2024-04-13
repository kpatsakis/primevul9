static void InitXCFImage(XCFLayerInfo *outLayer,ExceptionInfo *exception)
{
  outLayer->image->page.x=outLayer->offset_x;
  outLayer->image->page.y=outLayer->offset_y;
  outLayer->image->page.width=outLayer->width;
  outLayer->image->page.height=outLayer->height;
  (void) SetImageProperty(outLayer->image,"label",(char *)outLayer->name,
    exception);
}