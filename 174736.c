_ppdCacheGetMediaType(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    ipp_t        *job,			/* I - Job attributes or NULL */
    const char   *keyword)		/* I - Keyword string or NULL */
{
 /*
  * Range check input...
  */

  if (!pc || pc->num_types == 0 || (!job && !keyword))
    return (NULL);

  if (job && !keyword)
  {
   /*
    * Lookup the media-col attribute and any media-source found there...
    */

    ipp_attribute_t	*media_col,	/* media-col attribute */
			*media_type;	/* media-type attribute */

    media_col = ippFindAttribute(job, "media-col", IPP_TAG_BEGIN_COLLECTION);
    if (media_col)
    {
      if ((media_type = ippFindAttribute(media_col->values[0].collection,
                                         "media-type",
	                                 IPP_TAG_KEYWORD)) == NULL)
	media_type = ippFindAttribute(media_col->values[0].collection,
				      "media-type", IPP_TAG_NAME);

      if (media_type)
	keyword = media_type->values[0].string.text;
    }
  }

  if (keyword)
  {
    int	i;				/* Looping var */

    for (i = 0; i < pc->num_types; i ++)
      if (!_cups_strcasecmp(keyword, pc->types[i].pwg))
        return (pc->types[i].ppd);
  }

  return (NULL);
}