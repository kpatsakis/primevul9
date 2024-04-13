int imap_expand_path (BUFFER* path)
{
  IMAP_MBOX mx;
  IMAP_DATA* idata;
  ciss_url_t url;
  char fixedpath[LONG_STRING];
  int rc;

  if (imap_parse_path (mutt_b2s (path), &mx) < 0)
    return -1;

  idata = imap_conn_find (&mx.account, MUTT_IMAP_CONN_NONEW);
  mutt_account_tourl (&mx.account, &url);
  imap_fix_path (idata, mx.mbox, fixedpath, sizeof (fixedpath));
  url.path = fixedpath;

  rc = url_ciss_tobuffer (&url, path, U_DECODE_PASSWD);
  FREE (&mx.mbox);

  return rc;
}