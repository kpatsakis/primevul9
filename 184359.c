MonConnection::~MonConnection()
{
  if (con) {
    con->mark_down();
    con.reset();
  }
}