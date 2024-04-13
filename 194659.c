receive_add_recipient(uschar *recipient, int pno)
{
if (recipients_count >= recipients_list_max)
  {
  recipient_item *oldlist = recipients_list;
  int oldmax = recipients_list_max;
  recipients_list_max = recipients_list_max? 2*recipients_list_max : 50;
  recipients_list = store_get(recipients_list_max * sizeof(recipient_item));
  if (oldlist != NULL)
    memcpy(recipients_list, oldlist, oldmax * sizeof(recipient_item));
  }

recipients_list[recipients_count].address = recipient;
recipients_list[recipients_count].pno = pno;
#ifdef EXPERIMENTAL_BRIGHTMAIL
recipients_list[recipients_count].bmi_optin = bmi_current_optin;
/* reset optin string pointer for next recipient */
bmi_current_optin = NULL;
#endif
recipients_list[recipients_count].orcpt = NULL;
recipients_list[recipients_count].dsn_flags = 0;
recipients_list[recipients_count++].errors_to = NULL;
}