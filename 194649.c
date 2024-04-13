receive_check_set_sender(uschar *newsender)
{
uschar *qnewsender;
if (trusted_caller) return TRUE;
if (!newsender || !untrusted_set_sender) return FALSE;
qnewsender = Ustrchr(newsender, '@')
  ? newsender : string_sprintf("%s@%s", newsender, qualify_domain_sender);
return match_address_list_basic(qnewsender, CUSS &untrusted_set_sender, 0) == OK;
}