void default_bzfree ( void* opaque, void* addr )
{
   if (addr != NULL) free ( addr );
}