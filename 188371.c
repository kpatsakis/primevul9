void mutt_seqset_iterator_free (SEQSET_ITERATOR **p_iter)
{
  SEQSET_ITERATOR *iter;

  if (!p_iter || !*p_iter)
    return;

  iter = *p_iter;
  FREE (&iter->full_seqset);
  FREE (p_iter);               /* __FREE_CHECKED__ */
}