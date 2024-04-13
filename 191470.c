MagickExport void SetMagickAlignedMemoryMethods(
  AcquireAlignedMemoryHandler acquire_aligned_memory_handler,
  RelinquishAlignedMemoryHandler relinquish_aligned_memory_handler)
{
  memory_methods.acquire_aligned_memory_handler=acquire_aligned_memory_handler;
  memory_methods.relinquish_aligned_memory_handler=
      relinquish_aligned_memory_handler;
}