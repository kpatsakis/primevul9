int Dispatcher::param_count( void )
{
  if ( !parsed ) {
    parse_params();
  }

  return parsed_params.size();
}