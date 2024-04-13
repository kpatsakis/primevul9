bool MonClient::_hunting() const
{
  return !pending_cons.empty();
}