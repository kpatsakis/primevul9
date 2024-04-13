static void imap_generate_seqset (BUFFER *b, IMAP_DATA *idata, unsigned int msn_begin,
                                  unsigned int msn_end)
{
  int chunks = 0;
  int state = 0;  /* 1: single msn, 2: range of msn */
  unsigned int msn, range_begin, range_end;

  for (msn = msn_begin; msn <= msn_end + 1; msn++)
  {
    if (msn <= msn_end && !idata->msn_index[msn-1])
    {
      switch (state)
      {
        case 1:            /* single: convert to a range */
          state = 2;
          /* fall through */
        case 2:            /* extend range ending */
          range_end = msn;
          break;
        default:
          state = 1;
          range_begin = msn;
          break;
      }
    }
    else if (state)
    {
      if (chunks++)
        mutt_buffer_addch (b, ',');
      if (chunks == 150)
        break;

      if (state == 1)
        mutt_buffer_printf (b, "%u", range_begin);
      else if (state == 2)
        mutt_buffer_printf (b, "%u:%u", range_begin, range_end);
      state = 0;
    }
  }

  /* Too big.  Just query the whole range then. */
  if (chunks == 150 || mutt_strlen (b->data) > 500)
  {
    b->dptr = b->data;
    mutt_buffer_printf (b, "%u:%u", msn_begin, msn_end);
  }
}