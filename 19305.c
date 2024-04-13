update_card_removed (int vrdr, int value)
{
  struct server_local_s *sl;

  if (vrdr == -1)
    return;

  for (sl=session_list; sl; sl = sl->next_session)
    if (sl->ctrl_backlink
        && sl->ctrl_backlink->server_local->vreader_idx == vrdr)
      {
        sl->card_removed = value;
      }
  /* Let the card application layer know about the removal.  */
  if (value)
    application_notify_card_reset (vreader_slot (vrdr));
}