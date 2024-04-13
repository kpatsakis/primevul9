receive_remove_recipient(uschar *recipient)
{
int count;
DEBUG(D_receive) debug_printf("receive_remove_recipient(\"%s\") called\n",
  recipient);
for (count = 0; count < recipients_count; count++)
  {
  if (Ustrcmp(recipients_list[count].address, recipient) == 0)
    {
    if ((--recipients_count - count) > 0)
      memmove(recipients_list + count, recipients_list + count + 1,
        (recipients_count - count)*sizeof(recipient_item));
    return TRUE;
    }
  }
return FALSE;
}