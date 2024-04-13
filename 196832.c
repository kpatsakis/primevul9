void Dispatcher::clear( const Parser::Clear *act )
{
  params.clear();
  dispatch_chars.clear();
  parsed = false;
  act->handled = true;
}