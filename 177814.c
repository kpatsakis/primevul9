ffi_closure_free (void *ptr)
{
  void *codeseg, *dataseg;
  size_t rounded_size;

  dataseg = ADD_TO_POINTER(ptr, -overhead);
  memcpy(&rounded_size, dataseg, sizeof(rounded_size));
  memcpy(&codeseg, ADD_TO_POINTER(dataseg, sizeof(size_t)), sizeof(void *));
  munmap(dataseg, rounded_size);
  munmap(codeseg, rounded_size);
}