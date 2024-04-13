MagickExport MemoryInfo *AcquireVirtualMemory(const size_t count,
  const size_t quantum)
{
  char
    *value;

  MemoryInfo
    *memory_info;

  size_t
    size;

  if (HeapOverflowSanityCheckGetSize(count,quantum,&size) != MagickFalse)
    {
      errno=ENOMEM;
      return((MemoryInfo *) NULL);
    }
  if (virtual_anonymous_memory == 0)
    {
      virtual_anonymous_memory=1;
      value=GetPolicyValue("system:memory-map");
      if (LocaleCompare(value,"anonymous") == 0)
        {
          /*
            The security policy sets anonymous mapping for the memory request.
          */
#if defined(MAGICKCORE_HAVE_MMAP) && defined(MAP_ANONYMOUS)
          virtual_anonymous_memory=2;
#endif
        }
      value=DestroyString(value);
    }
  memory_info=(MemoryInfo *) MagickAssumeAligned(AcquireAlignedMemory(1,
    sizeof(*memory_info)));
  if (memory_info == (MemoryInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(memory_info,0,sizeof(*memory_info));
  memory_info->length=size;
  memory_info->signature=MagickCoreSignature;
  if ((virtual_anonymous_memory == 1) && (size <= GetMaxMemoryRequest()))
    {
      memory_info->blob=AcquireAlignedMemory(1,size);
      if (memory_info->blob != NULL)
        memory_info->type=AlignedVirtualMemory;
    }
  if (memory_info->blob == NULL)
    {
      /*
        Acquire anonymous memory map.
      */
      memory_info->blob=NULL;
      if (size <= GetMaxMemoryRequest())
        memory_info->blob=MapBlob(-1,IOMode,0,size);
      if (memory_info->blob != NULL)
        memory_info->type=MapVirtualMemory;
      else
        {
          int
            file;

          /*
            Anonymous memory mapping failed, try file-backed memory mapping.
          */
          file=AcquireUniqueFileResource(memory_info->filename);
          if (file != -1)
            {
              MagickOffsetType
                offset;

              offset=(MagickOffsetType) lseek(file,size-1,SEEK_SET);
              if ((offset == (MagickOffsetType) (size-1)) &&
                  (write(file,"",1) == 1))
                {
#if !defined(MAGICKCORE_HAVE_POSIX_FALLOCATE)
                  memory_info->blob=MapBlob(file,IOMode,0,size);
#else
                  if (posix_fallocate(file,0,(MagickOffsetType) size) == 0)
                    memory_info->blob=MapBlob(file,IOMode,0,size);
#endif
                  if (memory_info->blob != NULL)
                    memory_info->type=MapVirtualMemory;
                  else
                    {
                      (void) RelinquishUniqueFileResource(
                        memory_info->filename);
                      *memory_info->filename='\0';
                    }
                }
              (void) close(file);
            }
        }
    }
  if (memory_info->blob == NULL)
    {
      memory_info->blob=AcquireQuantumMemory(1,size);
      if (memory_info->blob != NULL)
        memory_info->type=UnalignedVirtualMemory;
    }
  if (memory_info->blob == NULL)
    memory_info=RelinquishVirtualMemory(memory_info);
  return(memory_info);
}