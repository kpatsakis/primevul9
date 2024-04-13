MagickExport MagickBooleanType XMakeImage(Display *display,
  const XResourceInfo *resource_info,XWindowInfo *window,Image *image,
  unsigned int width,unsigned int height)
{
#define CheckOverflowException(length,width,height) \
  (((height) != 0) && ((length)/((size_t) height) != ((size_t) width)))

  int
    depth,
    format;

  size_t
    length;

  XImage
    *matte_image,
    *ximage;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(width != 0);
  assert(height != 0);
  if ((window->width == 0) || (window->height == 0))
    return(MagickFalse);
  /*
    Apply user transforms to the image.
  */
  (void) XCheckDefineCursor(display,window->id,window->busy_cursor);
  (void) XFlush(display);
  depth=(int) window->depth;
  if (window->destroy)
    window->image=DestroyImage(window->image);
  window->image=image;
  window->destroy=MagickFalse;
  if (window->image != (Image *) NULL)
    {
      if (window->crop_geometry != (char *) NULL)
        {
          Image
            *crop_image;

          RectangleInfo
            crop_info;

          /*
            Crop image.
          */
          window->image->page.x=0;
          window->image->page.y=0;
          (void) ParsePageGeometry(window->image,window->crop_geometry,
            &crop_info,&image->exception);
          crop_image=CropImage(window->image,&crop_info,&image->exception);
          if (crop_image != (Image *) NULL)
            {
              if (window->image != image)
                window->image=DestroyImage(window->image);
              window->image=crop_image;
              window->destroy=MagickTrue;
            }
        }
      if ((width != (unsigned int) window->image->columns) ||
          (height != (unsigned int) window->image->rows))
        {
          Image
            *resize_image;

          /*
            Resize image.
          */
          resize_image=NewImageList();
          if ((window->pixel_info->colors == 0) &&
              (window->image->rows > (unsigned long) XDisplayHeight(display,window->screen)) &&
              (window->image->columns > (unsigned long) XDisplayWidth(display,window->screen)))
              resize_image=ResizeImage(window->image,width,height,
                image->filter,image->blur,&image->exception);
          else
            {
              if (window->image->storage_class == PseudoClass)
                resize_image=SampleImage(window->image,width,height,
                  &image->exception);
              else
                resize_image=ThumbnailImage(window->image,width,height,
                  &image->exception);
            }
          if (resize_image != (Image *) NULL)
            {
              if (window->image != image)
                window->image=DestroyImage(window->image);
              window->image=resize_image;
              window->destroy=MagickTrue;
            }
        }
      width=(unsigned int) window->image->columns;
      assert((size_t) width == window->image->columns);
      height=(unsigned int) window->image->rows;
      assert((size_t) height == window->image->rows);
    }
  /*
    Create X image.
  */
  ximage=(XImage *) NULL;
  format=(depth == 1) ? XYBitmap : ZPixmap;
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
  if (window->shared_memory != MagickFalse)
    {
      XShmSegmentInfo
        *segment_info;

      segment_info=(XShmSegmentInfo *) window->segment_info;
      segment_info[1].shmid=(-1);
      segment_info[1].shmaddr=(char *) NULL;
      ximage=XShmCreateImage(display,window->visual,(unsigned int) depth,format,
        (char *) NULL,&segment_info[1],width,height);
      if (ximage == (XImage *) NULL)
        window->shared_memory=MagickFalse;
      else
        {
          length=(size_t) ximage->bytes_per_line*ximage->height;
          if (CheckOverflowException(length,ximage->bytes_per_line,ximage->height))
            window->shared_memory=MagickFalse;
        }
      if (window->shared_memory != MagickFalse)
        segment_info[1].shmid=shmget(IPC_PRIVATE,length,IPC_CREAT | 0777);
      if (window->shared_memory != MagickFalse)
        segment_info[1].shmaddr=(char *) shmat(segment_info[1].shmid,0,0);
      if (segment_info[1].shmid < 0)
        window->shared_memory=MagickFalse;
      if (window->shared_memory != MagickFalse)
        (void) shmctl(segment_info[1].shmid,IPC_RMID,0);
      else
        {
          if (ximage != (XImage *) NULL)
            XDestroyImage(ximage);
          ximage=(XImage *) NULL;
          if (segment_info[1].shmaddr)
            {
              (void) shmdt(segment_info[1].shmaddr);
              segment_info[1].shmaddr=(char *) NULL;
            }
          if (segment_info[1].shmid >= 0)
            {
              (void) shmctl(segment_info[1].shmid,IPC_RMID,0);
              segment_info[1].shmid=(-1);
            }
        }
    }
#endif
  /*
    Allocate X image pixel data.
  */
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
  if (window->shared_memory)
    {
      Status
        status;

      XShmSegmentInfo
        *segment_info;

      (void) XSync(display,MagickFalse);
      xerror_alert=MagickFalse;
      segment_info=(XShmSegmentInfo *) window->segment_info;
      ximage->data=segment_info[1].shmaddr;
      segment_info[1].readOnly=MagickFalse;
      status=XShmAttach(display,&segment_info[1]);
      if (status != False)
        (void) XSync(display,MagickFalse);
      if ((status == False) || (xerror_alert != MagickFalse))
        {
          window->shared_memory=MagickFalse;
          if (status != False)
            XShmDetach(display,&segment_info[1]);
          ximage->data=NULL;
          XDestroyImage(ximage);
          ximage=(XImage *) NULL;
          if (segment_info[1].shmid >= 0)
            {
              if (segment_info[1].shmaddr != NULL)
                (void) shmdt(segment_info[1].shmaddr);
              (void) shmctl(segment_info[1].shmid,IPC_RMID,0);
              segment_info[1].shmid=(-1);
              segment_info[1].shmaddr=(char *) NULL;
            }
        }
    }
#endif
  if (window->shared_memory == MagickFalse)
    ximage=XCreateImage(display,window->visual,(unsigned int) depth,format,0,
      (char *) NULL,width,height,XBitmapPad(display),0);
  if (ximage == (XImage *) NULL)
    {
      /*
        Unable to create X image.
      */
      (void) XCheckDefineCursor(display,window->id,window->cursor);
      return(MagickFalse);
    }
  length=(size_t) ximage->bytes_per_line*ximage->height;
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),"XImage:");
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  width, height: %dx%d",
        ximage->width,ximage->height);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  format: %d",
        ximage->format);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  byte order: %d",
        ximage->byte_order);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  bitmap unit, bit order, pad: %d %d %d",ximage->bitmap_unit,
        ximage->bitmap_bit_order,ximage->bitmap_pad);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  depth: %d",
        ximage->depth);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  bytes per line: %d",
        ximage->bytes_per_line);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  bits per pixel: %d",
        ximage->bits_per_pixel);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  red, green, blue masks: 0x%lx 0x%lx 0x%lx",ximage->red_mask,
        ximage->green_mask,ximage->blue_mask);
    }
  if (window->shared_memory == MagickFalse)
    {
      if (ximage->format == XYBitmap)
        {
          ximage->data=(char *) AcquireQuantumMemory((size_t)
            ximage->bytes_per_line,(size_t) ximage->depth*ximage->height);
          if (ximage->data != (char *) NULL)
            (void) memset(ximage->data,0,(size_t)
              ximage->bytes_per_line*ximage->depth*ximage->height);
        }
      else
        {
          ximage->data=(char *) AcquireQuantumMemory((size_t)
            ximage->bytes_per_line,(size_t) ximage->height);
          if (ximage->data != (char *) NULL)
            (void) memset(ximage->data,0,(size_t)
              ximage->bytes_per_line*ximage->height);
        }
    }
  if (ximage->data == (char *) NULL)
    {
      /*
        Unable to allocate pixel data.
      */
      XDestroyImage(ximage);
      ximage=(XImage *) NULL;
      (void) XCheckDefineCursor(display,window->id,window->cursor);
      return(MagickFalse);
    }
  if (window->ximage != (XImage *) NULL)
    {
      /*
        Destroy previous X image.
      */
      length=(size_t) window->ximage->bytes_per_line*window->ximage->height;
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
      if (window->segment_info != (XShmSegmentInfo *) NULL)
        {
          XShmSegmentInfo
            *segment_info;

          segment_info=(XShmSegmentInfo *) window->segment_info;
          if (segment_info[0].shmid >= 0)
            {
              (void) XSync(display,MagickFalse);
              (void) XShmDetach(display,&segment_info[0]);
              (void) XSync(display,MagickFalse);
              if (segment_info[0].shmaddr != (char *) NULL)
                (void) shmdt(segment_info[0].shmaddr);
              (void) shmctl(segment_info[0].shmid,IPC_RMID,0);
              segment_info[0].shmid=(-1);
              segment_info[0].shmaddr=(char *) NULL;
              window->ximage->data=(char *) NULL;
          }
        }
#endif
      if (window->ximage->data != (char *) NULL)
        free(window->ximage->data);
      window->ximage->data=(char *) NULL;
      XDestroyImage(window->ximage);
      window->ximage=(XImage *) NULL;
    }
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
  if (window->segment_info != (XShmSegmentInfo *) NULL)
    {
      XShmSegmentInfo
        *segment_info;

      segment_info=(XShmSegmentInfo *) window->segment_info;
      segment_info[0]=segment_info[1];
    }
#endif
  window->ximage=ximage;
  matte_image=(XImage *) NULL;
  if ((window->shape != MagickFalse) && (window->image != (Image *) NULL))
    if ((window->image->matte != MagickFalse) &&
        ((int) width <= XDisplayWidth(display,window->screen)) &&
        ((int) height <= XDisplayHeight(display,window->screen)))
      {
        /*
          Create matte image.
        */
        matte_image=XCreateImage(display,window->visual,1,XYBitmap,0,
          (char *) NULL,width,height,XBitmapPad(display),0);
        if (IsEventLogging())
          {
            (void) LogMagickEvent(X11Event,GetMagickModule(),"Matte Image:");
            (void) LogMagickEvent(X11Event,GetMagickModule(),
              "  width, height: %dx%d",matte_image->width,matte_image->height);
          }
        if (matte_image != (XImage *) NULL)
          {
            /*
              Allocate matte image pixel data.
            */
            matte_image->data=(char *) malloc((size_t)
              matte_image->bytes_per_line*matte_image->depth*
              matte_image->height);
            if (matte_image->data == (char *) NULL)
              {
                XDestroyImage(matte_image);
                matte_image=(XImage *) NULL;
              }
          }
      }
  if (window->matte_image != (XImage *) NULL)
    {
      /*
        Free matte image.
      */
      if (window->matte_image->data != (char *) NULL)
        free(window->matte_image->data);
      window->matte_image->data=(char *) NULL;
      XDestroyImage(window->matte_image);
      window->matte_image=(XImage *) NULL;
    }
  window->matte_image=matte_image;
  if (window->matte_pixmap != (Pixmap) NULL)
    {
      (void) XFreePixmap(display,window->matte_pixmap);
      window->matte_pixmap=(Pixmap) NULL;
#if defined(MAGICKCORE_HAVE_SHAPE)
      if (window->shape != MagickFalse)
        XShapeCombineMask(display,window->id,ShapeBounding,0,0,None,ShapeSet);
#endif
    }
  window->stasis=MagickFalse;
  /*
    Convert pixels to X image data.
  */
  if (window->image != (Image *) NULL)
    {
      if ((ximage->byte_order == LSBFirst) || ((ximage->format == XYBitmap) &&
          (ximage->bitmap_bit_order == LSBFirst)))
        XMakeImageLSBFirst(resource_info,window,window->image,ximage,
          matte_image);
      else
        XMakeImageMSBFirst(resource_info,window,window->image,ximage,
          matte_image);
    }
  if (window->matte_image != (XImage *) NULL)
    {
      /*
        Create matte pixmap.
      */
      window->matte_pixmap=XCreatePixmap(display,window->id,width,height,1);
      if (window->matte_pixmap != (Pixmap) NULL)
        {
          GC
            graphics_context;

          XGCValues
            context_values;

          /*
            Copy matte image to matte pixmap.
          */
          context_values.background=1;
          context_values.foreground=0;
          graphics_context=XCreateGC(display,window->matte_pixmap,
            (size_t) (GCBackground | GCForeground),&context_values);
          (void) XPutImage(display,window->matte_pixmap,graphics_context,
            window->matte_image,0,0,0,0,width,height);
          (void) XFreeGC(display,graphics_context);
#if defined(MAGICKCORE_HAVE_SHAPE)
          if (window->shape != MagickFalse)
            XShapeCombineMask(display,window->id,ShapeBounding,0,0,
              window->matte_pixmap,ShapeSet);
#endif
        }
      }
  (void) XMakePixmap(display,resource_info,window);
  /*
    Restore cursor.
  */
  (void) XCheckDefineCursor(display,window->id,window->cursor);
  return(MagickTrue);
}