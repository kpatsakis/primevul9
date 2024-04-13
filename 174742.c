pwg_compare_finishings(
    _pwg_finishings_t *a,		/* I - First finishings value */
    _pwg_finishings_t *b)		/* I - Second finishings value */
{
  return ((int)b->value - (int)a->value);
}