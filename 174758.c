_ppdCacheGetSource(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    const char   *input_slot)		/* I - PPD InputSlot */
{
  int		i;			/* Looping var */
  pwg_map_t	*source;		/* Current source */


 /*
  * Range check input...
  */

  if (!pc || !input_slot)
    return (NULL);

  for (i = pc->num_sources, source = pc->sources; i > 0; i --, source ++)
    if (!_cups_strcasecmp(input_slot, source->ppd))
      return (source->pwg);

  return (NULL);
}