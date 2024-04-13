_ppdCacheGetFinishingValues(
    _ppd_cache_t  *pc,			/* I - PPD cache and mapping data */
    int           num_options,		/* I - Number of options */
    cups_option_t *options,		/* I - Options */
    int           max_values,		/* I - Maximum number of finishings values */
    int           *values)		/* O - Finishings values */
{
  int			i,		/* Looping var */
			num_values = 0;	/* Number of values */
  _pwg_finishings_t	*f;		/* Current finishings option */
  cups_option_t		*option;	/* Current option */
  const char		*val;		/* Value for option */


 /*
  * Range check input...
  */

  DEBUG_printf(("_ppdCacheGetFinishingValues(pc=%p, num_options=%d, options=%p, max_values=%d, values=%p)", pc, num_options, options, max_values, values));

  if (!pc || !pc->finishings || num_options < 1 || max_values < 1 || !values)
  {
    DEBUG_puts("_ppdCacheGetFinishingValues: Bad arguments, returning 0.");
    return (0);
  }

 /*
  * Go through the finishings options and see what is set...
  */

  for (f = (_pwg_finishings_t *)cupsArrayFirst(pc->finishings);
       f;
       f = (_pwg_finishings_t *)cupsArrayNext(pc->finishings))
  {
    DEBUG_printf(("_ppdCacheGetFinishingValues: Checking %d (%s)", f->value, ippEnumString("finishings", f->value)));

    for (i = f->num_options, option = f->options; i > 0; i --, option ++)
    {
      DEBUG_printf(("_ppdCacheGetFinishingValues: %s=%s?", option->name, option->value));

      if ((val = cupsGetOption(option->name, num_options, options)) == NULL ||
          _cups_strcasecmp(option->value, val))
      {
        DEBUG_puts("_ppdCacheGetFinishingValues: NO");
        break;
      }
    }

    if (i == 0)
    {
      DEBUG_printf(("_ppdCacheGetFinishingValues: Adding %d.", f->value));

      values[num_values ++] = f->value;

      if (num_values >= max_values)
        break;
    }
  }

  DEBUG_printf(("_ppdCacheGetFinishingValues: Returning %d.", num_values));

  return (num_values);
}