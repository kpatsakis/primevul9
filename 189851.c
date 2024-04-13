void imap_free_header_data (IMAP_HEADER_DATA** data)
{
  if (*data)
  {
    /* this should be safe even if the list wasn't used */
    mutt_free_list (&((*data)->keywords));
    FREE (data); /* __FREE_CHECKED__ */
  }
}