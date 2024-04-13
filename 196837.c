void Dispatcher::collect( const Parser::Collect *act )
{
  assert( act->char_present );
  if ( ( dispatch_chars.length() < 8 ) /* never should need more than 2 */
       && ( act->ch <= 255 ) ) {  /* ignore non-8-bit */    
    dispatch_chars.push_back( act->ch );
    act->handled = true;
  }
}