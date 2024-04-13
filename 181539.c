MagickExport StreamInfo *DestroyStreamInfo(StreamInfo *stream_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  if (stream_info->map != (char *) NULL)
    stream_info->map=DestroyString(stream_info->map);
  if (stream_info->pixels != (unsigned char *) NULL)
    stream_info->pixels=(unsigned char *) RelinquishAlignedMemory(
      stream_info->pixels);
  if (stream_info->stream != (Image *) NULL)
    {
      (void) CloseBlob(stream_info->stream);
      stream_info->stream=DestroyImage(stream_info->stream);
    }
  if (stream_info->quantum_info != (QuantumInfo *) NULL)
    stream_info->quantum_info=DestroyQuantumInfo(stream_info->quantum_info);
  stream_info->signature=(~MagickCoreSignature);
  stream_info=(StreamInfo *) RelinquishMagickMemory(stream_info);
  return(stream_info);
}