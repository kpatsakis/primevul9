qtdemux_zfree (void *opaque, void *addr)
{
  g_free (addr);
}