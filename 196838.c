static void register_function( Function_Type type,
			       std::string dispatch_chars,
			       Function f )
{
  switch ( type ) {
  case ESCAPE:
    get_global_dispatch_registry().escape.insert( dispatch_map_t::value_type( dispatch_chars, f ) );
    break;
  case CSI:
    get_global_dispatch_registry().CSI.insert( dispatch_map_t::value_type( dispatch_chars, f ) );
    break;
  case CONTROL:
    get_global_dispatch_registry().control.insert( dispatch_map_t::value_type( dispatch_chars, f ) );
    break;
  }
}