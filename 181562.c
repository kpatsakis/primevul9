MagickPrivate const void *GetStreamInfoClientData(StreamInfo *stream_info)
{
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  return(stream_info->client_data);
}