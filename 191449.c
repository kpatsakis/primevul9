MagickExport void *RelinquishAlignedMemory(void *memory)
{
  if (memory == (void *) NULL)
    return((void *) NULL);
  if (memory_methods.relinquish_aligned_memory_handler != (RelinquishAlignedMemoryHandler) NULL)
    {
      memory_methods.relinquish_aligned_memory_handler(memory);
      return(NULL);
    }
#if defined(MAGICKCORE_HAVE_ALIGNED_MALLOC) || defined(MAGICKCORE_HAVE_POSIX_MEMALIGN)
  free(memory);
#elif defined(MAGICKCORE_HAVE__ALIGNED_MALLOC)
  _aligned_free(memory);
#else
  RelinquishMagickMemory(actual_base_address(memory));
#endif
  return(NULL);
}