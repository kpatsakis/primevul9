void imap_buffer_qualify_path (BUFFER *dest, IMAP_MBOX *mx, char* path)
{
  ciss_url_t url;

  mutt_account_tourl (&mx->account, &url);
  url.path = path;

  url_ciss_tobuffer (&url, dest, 0);
}