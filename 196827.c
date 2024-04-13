bool Dispatcher::operator==( const Dispatcher &x ) const
{
  return ( params == x.params ) && ( parsed_params == x.parsed_params ) && ( parsed == x.parsed )
    && ( dispatch_chars == x.dispatch_chars ) && ( OSC_string == x.OSC_string ) && ( terminal_to_host == x.terminal_to_host );
}