MagickExport MemoryInfo *RelinquishVirtualMemory(MemoryInfo *memory_info)
{
  assert(memory_info != (MemoryInfo *) NULL);
  assert(memory_info->signature == MagickCoreSignature);
  if (memory_info->blob != (void *) NULL)
    switch (memory_info->type)
    {
      case AlignedVirtualMemory:
      {
        memory_info->blob=RelinquishAlignedMemory(memory_info->blob);
        break;
      }
      case MapVirtualMemory:
      {
        (void) UnmapBlob(memory_info->blob,memory_info->length);
        memory_info->blob=NULL;
        if (*memory_info->filename != '\0')
          (void) RelinquishUniqueFileResource(memory_info->filename);
        break;
      }
      case UnalignedVirtualMemory:
      default:
      {
        memory_info->blob=RelinquishMagickMemory(memory_info->blob);
        break;
      }
    }
  memory_info->signature=(~MagickCoreSignature);
  memory_info=(MemoryInfo *) RelinquishAlignedMemory(memory_info);
  return(memory_info);
}