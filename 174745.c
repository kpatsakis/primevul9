_ppdCacheGetType(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    const char   *media_type)		/* I - PPD MediaType */
{
  int		i;			/* Looping var */
  pwg_map_t	*type;			/* Current type */


 /*
  * Range check input...
  */

  if (!pc || !media_type)
    return (NULL);

  for (i = pc->num_types, type = pc->types; i > 0; i --, type ++)
    if (!_cups_strcasecmp(media_type, type->ppd))
      return (type->pwg);

  return (NULL);
}