qtdemux_zalloc (void *opaque, unsigned int items, unsigned int size)
{
  return g_malloc (items * size);
}