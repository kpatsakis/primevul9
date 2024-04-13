_ppdCacheGetFinishingOptions(
    _ppd_cache_t     *pc,		/* I  - PPD cache and mapping data */
    ipp_t            *job,		/* I  - Job attributes or NULL */
    ipp_finishings_t value,		/* I  - IPP finishings value of IPP_FINISHINGS_NONE */
    int              num_options,	/* I  - Number of options */
    cups_option_t    **options)		/* IO - Options */
{
  int			i;		/* Looping var */
  _pwg_finishings_t	*f,		/* PWG finishings options */
			key;		/* Search key */
  ipp_attribute_t	*attr;		/* Finishings attribute */
  cups_option_t		*option;	/* Current finishings option */


 /*
  * Range check input...
  */

  if (!pc || cupsArrayCount(pc->finishings) == 0 || !options ||
      (!job && value == IPP_FINISHINGS_NONE))
    return (num_options);

 /*
  * Apply finishing options...
  */

  if (job && (attr = ippFindAttribute(job, "finishings", IPP_TAG_ENUM)) != NULL)
  {
    int	num_values = ippGetCount(attr);	/* Number of values */

    for (i = 0; i < num_values; i ++)
    {
      key.value = (ipp_finishings_t)ippGetInteger(attr, i);

      if ((f = cupsArrayFind(pc->finishings, &key)) != NULL)
      {
        int	j;			/* Another looping var */

        for (j = f->num_options, option = f->options; j > 0; j --, option ++)
          num_options = cupsAddOption(option->name, option->value,
                                      num_options, options);
      }
    }
  }
  else if (value != IPP_FINISHINGS_NONE)
  {
    key.value = value;

    if ((f = cupsArrayFind(pc->finishings, &key)) != NULL)
    {
      int	j;			/* Another looping var */

      for (j = f->num_options, option = f->options; j > 0; j --, option ++)
	num_options = cupsAddOption(option->name, option->value,
				    num_options, options);
    }
  }

  return (num_options);
}