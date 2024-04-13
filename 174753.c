_ppdCacheGetInputSlot(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    ipp_t        *job,			/* I - Job attributes or NULL */
    const char   *keyword)		/* I - Keyword string or NULL */
{
 /*
  * Range check input...
  */

  if (!pc || pc->num_sources == 0 || (!job && !keyword))
    return (NULL);

  if (job && !keyword)
  {
   /*
    * Lookup the media-col attribute and any media-source found there...
    */

    ipp_attribute_t	*media_col,	/* media-col attribute */
			*media_source;	/* media-source attribute */
    pwg_size_t		size;		/* Dimensional size */
    int			margins_set;	/* Were the margins set? */

    media_col = ippFindAttribute(job, "media-col", IPP_TAG_BEGIN_COLLECTION);
    if (media_col &&
        (media_source = ippFindAttribute(ippGetCollection(media_col, 0),
                                         "media-source",
	                                 IPP_TAG_KEYWORD)) != NULL)
    {
     /*
      * Use the media-source value from media-col...
      */

      keyword = ippGetString(media_source, 0, NULL);
    }
    else if (pwgInitSize(&size, job, &margins_set))
    {
     /*
      * For media <= 5x7, look for a photo tray...
      */

      if (size.width <= (5 * 2540) && size.length <= (7 * 2540))
        keyword = "photo";
    }
  }

  if (keyword)
  {
    int	i;				/* Looping var */

    for (i = 0; i < pc->num_sources; i ++)
      if (!_cups_strcasecmp(keyword, pc->sources[i].pwg))
        return (pc->sources[i].ppd);
  }

  return (NULL);
}