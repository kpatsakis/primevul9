void* default_bzalloc ( void* opaque, Int32 items, Int32 size )
{
   void* v = cli_malloc ( items * size );
   return v;
}