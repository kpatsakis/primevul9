static void imap_msn_index_to_uid_seqset (BUFFER *b, IMAP_DATA *idata)
{
  int first = 1, state = 0, match = 0;
  HEADER *cur_header;
  unsigned int msn, cur_uid = 0, last_uid = 0;
  unsigned int range_begin = 0, range_end = 0;

  for (msn = 1; msn <= idata->max_msn + 1; msn++)
  {
    match = 0;
    if (msn <= idata->max_msn)
    {
      cur_header = idata->msn_index[msn - 1];
      cur_uid = cur_header ? HEADER_DATA(cur_header)->uid : 0;
      if (!state || (cur_uid && (cur_uid - 1 == last_uid)))
        match = 1;
      last_uid = cur_uid;
    }

    if (match)
    {
      switch (state)
      {
        case 1:            /* single: convert to a range */
          state = 2;
          /* fall through */
        case 2:            /* extend range ending */
          range_end = cur_uid;
          break;
        default:
          state = 1;
          range_begin = cur_uid;
          break;
      }
    }
    else if (state)
    {
      if (first)
        first = 0;
      else
        mutt_buffer_addch (b, ',');

      if (state == 1)
        mutt_buffer_add_printf (b, "%u", range_begin);
      else if (state == 2)
        mutt_buffer_add_printf (b, "%u:%u", range_begin, range_end);

      state = 1;
      range_begin = cur_uid;
    }
  }
}