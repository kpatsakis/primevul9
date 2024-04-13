SEQSET_ITERATOR *mutt_seqset_iterator_new (const char *seqset)
{
  SEQSET_ITERATOR *iter;

  if (!seqset || !*seqset)
    return NULL;

  iter = safe_calloc (1, sizeof(SEQSET_ITERATOR));
  iter->full_seqset = safe_strdup (seqset);
  iter->eostr = strchr (iter->full_seqset, '\0');
  iter->substr_cur = iter->substr_end = iter->full_seqset;

  return iter;
}