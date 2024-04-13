_ppdCacheGetOutputBin(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    const char   *output_bin)		/* I - Keyword string */
{
  int	i;				/* Looping var */


 /*
  * Range check input...
  */

  if (!pc || !output_bin)
    return (NULL);

 /*
  * Look up the OutputBin string...
  */


  for (i = 0; i < pc->num_bins; i ++)
    if (!_cups_strcasecmp(output_bin, pc->bins[i].pwg))
      return (pc->bins[i].ppd);

  return (NULL);
}