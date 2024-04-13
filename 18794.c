update_tcap(int attr)
{
    struct builtin_term *p;

    p = find_builtin_term(DEFAULT_TERM);
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    while (p->bt_string != NULL)
    {
      if (p->bt_entry == (int)KS_ME)
	  p->bt_string = &ksme_str[0];
      else if (p->bt_entry == (int)KS_MR)
	  p->bt_string = &ksmr_str[0];
      else if (p->bt_entry == (int)KS_MD)
	  p->bt_string = &ksmd_str[0];
      ++p;
    }
}