IMAP_DATA* imap_new_idata (void)
{
  IMAP_DATA* idata = safe_calloc (1, sizeof (IMAP_DATA));

  idata->cmdbuf = mutt_buffer_new ();
  idata->cmdslots = ImapPipelineDepth + 2;
  idata->cmds = safe_calloc (idata->cmdslots, sizeof(*idata->cmds));

  return idata;
}