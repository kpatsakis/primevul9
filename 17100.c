check_export_name (uint32_t option, char *buf,
                   uint32_t exportnamelen, uint32_t maxlen)
{
  GET_CONN;

  if (check_string (option, buf, exportnamelen, maxlen, "export name") == -1)
    return -1;

  debug ("newstyle negotiation: %s: client requested export '%.*s'",
         name_of_nbd_opt (option), (int) exportnamelen, buf);
  return 0;
}