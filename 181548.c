MagickPrivate void SetStreamInfoClientData(StreamInfo *stream_info,
  const void *client_data)
{
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  stream_info->client_data=client_data;
}