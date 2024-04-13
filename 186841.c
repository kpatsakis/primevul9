static FriBidiRun *get_adjacent_run(FriBidiRun *list, fribidi_boolean forward, fribidi_boolean skip_neutral)
{
  FriBidiRun *ppp = forward ? list->next_isolate : list->prev_isolate;
  if (!ppp)
    return &sentinel;

  while (ppp)
    {
      FriBidiCharType ppp_type = RL_TYPE (ppp);

      if (ppp_type == FRIBIDI_TYPE_SENTINEL)
        break;

      /* Note that when sweeping forward we continue one run
         beyond the PDI to see what lies behind. When looking
         backwards, this is not necessary as the leading isolate
         run has already been assigned the resolved level. */
      if (ppp->isolate_level > list->isolate_level   /* <- How can this be true? */
          || (forward && ppp_type == FRIBIDI_TYPE_PDI)
          || (skip_neutral && !FRIBIDI_IS_STRONG(ppp_type)))
        {
          ppp = forward ? ppp->next_isolate : ppp->prev_isolate;
          if (!ppp)
            ppp = &sentinel;

          continue;
        }
      break;
    }

  return ppp;
}