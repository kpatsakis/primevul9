reallocWrapper (void *address, size_t size)
{
  if (!(address = realloc (address, size)) && size)
    outOfMemory ();
  return address;
}