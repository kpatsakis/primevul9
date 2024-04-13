_ppdCacheGetSize(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    const char   *page_size)		/* I - PPD PageSize */
{
  int		i;			/* Looping var */
  pwg_media_t	*media;			/* Media */
  pwg_size_t	*size;			/* Current size */


 /*
  * Range check input...
  */

  if (!pc || !page_size)
    return (NULL);

  if (!_cups_strncasecmp(page_size, "Custom.", 7))
  {
   /*
    * Custom size; size name can be one of the following:
    *
    *    Custom.WIDTHxLENGTHin    - Size in inches
    *    Custom.WIDTHxLENGTHft    - Size in feet
    *    Custom.WIDTHxLENGTHcm    - Size in centimeters
    *    Custom.WIDTHxLENGTHmm    - Size in millimeters
    *    Custom.WIDTHxLENGTHm     - Size in meters
    *    Custom.WIDTHxLENGTH[pt]  - Size in points
    */

    double		w, l;		/* Width and length of page */
    char		*ptr;		/* Pointer into PageSize */
    struct lconv	*loc;		/* Locale data */

    loc = localeconv();
    w   = (float)_cupsStrScand(page_size + 7, &ptr, loc);
    if (!ptr || *ptr != 'x')
      return (NULL);

    l = (float)_cupsStrScand(ptr + 1, &ptr, loc);
    if (!ptr)
      return (NULL);

    if (!_cups_strcasecmp(ptr, "in"))
    {
      w *= 2540.0;
      l *= 2540.0;
    }
    else if (!_cups_strcasecmp(ptr, "ft"))
    {
      w *= 12.0 * 2540.0;
      l *= 12.0 * 2540.0;
    }
    else if (!_cups_strcasecmp(ptr, "mm"))
    {
      w *= 100.0;
      l *= 100.0;
    }
    else if (!_cups_strcasecmp(ptr, "cm"))
    {
      w *= 1000.0;
      l *= 1000.0;
    }
    else if (!_cups_strcasecmp(ptr, "m"))
    {
      w *= 100000.0;
      l *= 100000.0;
    }
    else
    {
      w *= 2540.0 / 72.0;
      l *= 2540.0 / 72.0;
    }

    pc->custom_size.width  = (int)w;
    pc->custom_size.length = (int)l;

    return (&(pc->custom_size));
  }

 /*
  * Not a custom size - look it up...
  */

  for (i = pc->num_sizes, size = pc->sizes; i > 0; i --, size ++)
    if (!_cups_strcasecmp(page_size, size->map.ppd) ||
        !_cups_strcasecmp(page_size, size->map.pwg))
      return (size);

 /*
  * Look up standard sizes...
  */

  if ((media = pwgMediaForPPD(page_size)) == NULL)
    if ((media = pwgMediaForLegacy(page_size)) == NULL)
      media = pwgMediaForPWG(page_size);

  if (media)
  {
    pc->custom_size.width  = media->width;
    pc->custom_size.length = media->length;

    return (&(pc->custom_size));
  }

  return (NULL);
}