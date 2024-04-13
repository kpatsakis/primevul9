MagickExport Image *AutoOrientImage(const Image *image,
  const OrientationType orientation,ExceptionInfo *exception)
{
  Image
    *orient_image;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  orient_image=(Image *) NULL;
  switch(orientation)
  {
    case UndefinedOrientation:
    case TopLeftOrientation:
    default:
    {
      orient_image=CloneImage(image,0,0,MagickTrue,exception);
      break;
    }
    case TopRightOrientation:
    {
      orient_image=FlopImage(image,exception);
      break;
    }
    case BottomRightOrientation:
    {
      orient_image=RotateImage(image,180.0,exception);
      break;
    }
    case BottomLeftOrientation:
    {
      orient_image=FlipImage(image,exception);
      break;
    }
    case LeftTopOrientation:
    {
      orient_image=TransposeImage(image,exception);
      break;
    }
    case RightTopOrientation:
    {
      orient_image=RotateImage(image,90.0,exception);
      break;
    }
    case RightBottomOrientation:
    {
      orient_image=TransverseImage(image,exception);
      break;
    }
    case LeftBottomOrientation:
    {
      orient_image=RotateImage(image,270.0,exception);
      break;
    }
  }
  if (orient_image != (Image *) NULL)
    orient_image->orientation=TopLeftOrientation;
  return(orient_image);
}