void Dispatcher::OSC_start( const Parser::OSC_Start *act )
{
  OSC_string.clear();
  act->handled = true;
}