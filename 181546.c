MagickExport Image *StreamImage(const ImageInfo *image_info,
  StreamInfo *stream_info,ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *read_info;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  read_info=CloneImageInfo(image_info);
  stream_info->image_info=image_info;
  stream_info->quantum_info=AcquireQuantumInfo(image_info,(Image *) NULL);
  stream_info->exception=exception;
  read_info->client_data=(void *) stream_info;
  image=ReadStream(read_info,&WriteStreamImage,exception);
  read_info=DestroyImageInfo(read_info);
  stream_info->quantum_info=DestroyQuantumInfo(stream_info->quantum_info);
  stream_info->quantum_info=AcquireQuantumInfo(image_info,image);
  if (stream_info->quantum_info == (QuantumInfo *) NULL)
    image=DestroyImage(image);
  return(image);
}