bool MonConnection::have_session() const
{
  return state == State::HAVE_SESSION;
}