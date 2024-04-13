MagickExport void SetStreamInfoMap(StreamInfo *stream_info,const char *map)
{
  assert(stream_info != (StreamInfo *) NULL);
  assert(stream_info->signature == MagickCoreSignature);
  (void) CloneString(&stream_info->map,map);
}