static void enriched_set_flags (const wchar_t *tag, struct enriched_state *stte)
{
  const wchar_t *tagptr = tag;
  int i, j;

  if (*tagptr == (wchar_t) '/')
    tagptr++;

  for (i = 0, j = -1; EnrichedTags[i].tag_name; i++)
    if (wcscasecmp (EnrichedTags[i].tag_name, tagptr) == 0)
    {
      j = EnrichedTags[i].index;
      break;
    }

  if (j != -1)
  {
    if (j == RICH_CENTER || j == RICH_FLUSHLEFT || j == RICH_FLUSHRIGHT)
      enriched_flush (stte, 1);

    if (*tag == (wchar_t) '/')
    {
      if (stte->tag_level[j]) /* make sure not to go negative */
	stte->tag_level[j]--;
      if ((stte->s->flags & MUTT_DISPLAY) && j == RICH_PARAM && stte->tag_level[RICH_COLOR])
      {
	stte->param[stte->param_used] = (wchar_t) '\0';
	if (!wcscasecmp(L"black", stte->param))
	{
	  enriched_puts("\033[30m", stte);
	}
	else if (!wcscasecmp(L"red", stte->param))
	{
	  enriched_puts("\033[31m", stte);
	}
	else if (!wcscasecmp(L"green", stte->param))
	{
	  enriched_puts("\033[32m", stte);
	}
	else if (!wcscasecmp(L"yellow", stte->param))
	{
	  enriched_puts("\033[33m", stte);
	}
	else if (!wcscasecmp(L"blue", stte->param))
	{
	  enriched_puts("\033[34m", stte);
	}
	else if (!wcscasecmp(L"magenta", stte->param))
	{
	  enriched_puts("\033[35m", stte);
	}
	else if (!wcscasecmp(L"cyan", stte->param))
	{
	  enriched_puts("\033[36m", stte);
	}
	else if (!wcscasecmp(L"white", stte->param))
	{
	  enriched_puts("\033[37m", stte);
	}
      }
      if ((stte->s->flags & MUTT_DISPLAY) && j == RICH_COLOR)
      {
	enriched_puts("\033[0m", stte);
      }

      /* flush parameter buffer when closing the tag */
      if (j == RICH_PARAM)
      {
	stte->param_used = 0;
	stte->param[0] = (wchar_t) '\0';
      }
    }
    else
      stte->tag_level[j]++;

    if (j == RICH_EXCERPT)
      enriched_flush(stte, 1);
  }
}