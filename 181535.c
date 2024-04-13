MagickExport void SetStreamInfoStorageType(StreamInfo *stream_info,
  const StorageType storage_type)
{
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  stream_info->storage_type=storage_type;
}