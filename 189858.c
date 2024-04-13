void imap_add_keywords (char* s, HEADER* h, LIST* mailbox_flags, size_t slen)
{
  LIST *keywords;

  if (!mailbox_flags || !HEADER_DATA(h) || !HEADER_DATA(h)->keywords)
    return;

  keywords = HEADER_DATA(h)->keywords->next;

  while (keywords)
  {
    if (imap_has_flag (mailbox_flags, keywords->data))
    {
      safe_strcat (s, slen, keywords->data);
      safe_strcat (s, slen, " ");
    }
    keywords = keywords->next;
  }
}