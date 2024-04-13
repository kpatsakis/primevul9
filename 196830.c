DispatchRegistry & Terminal::get_global_dispatch_registry( void )
{
  static DispatchRegistry global_dispatch_registry;
  return global_dispatch_registry;
}