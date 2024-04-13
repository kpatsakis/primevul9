std::string Dispatcher::str( void )
{
  char assum[ 64 ];
  snprintf( assum, 64, "[dispatch=\"%s\" params=\"%s\"]",
	    dispatch_chars.c_str(), params.c_str() );
  return std::string( assum );
}