_ppdCacheDestroy(_ppd_cache_t *pc)	/* I - PPD cache and mapping data */
{
  int		i;			/* Looping var */
  pwg_map_t	*map;			/* Current map */
  pwg_size_t	*size;			/* Current size */


 /*
  * Range check input...
  */

  if (!pc)
    return;

 /*
  * Free memory as needed...
  */

  if (pc->bins)
  {
    for (i = pc->num_bins, map = pc->bins; i > 0; i --, map ++)
    {
      _cupsStrFree(map->pwg);
      _cupsStrFree(map->ppd);
    }

    free(pc->bins);
  }

  if (pc->sizes)
  {
    for (i = pc->num_sizes, size = pc->sizes; i > 0; i --, size ++)
    {
      _cupsStrFree(size->map.pwg);
      _cupsStrFree(size->map.ppd);
    }

    free(pc->sizes);
  }

  if (pc->source_option)
    _cupsStrFree(pc->source_option);

  if (pc->sources)
  {
    for (i = pc->num_sources, map = pc->sources; i > 0; i --, map ++)
    {
      _cupsStrFree(map->pwg);
      _cupsStrFree(map->ppd);
    }

    free(pc->sources);
  }

  if (pc->types)
  {
    for (i = pc->num_types, map = pc->types; i > 0; i --, map ++)
    {
      _cupsStrFree(map->pwg);
      _cupsStrFree(map->ppd);
    }

    free(pc->types);
  }

  if (pc->custom_max_keyword)
    _cupsStrFree(pc->custom_max_keyword);

  if (pc->custom_min_keyword)
    _cupsStrFree(pc->custom_min_keyword);

  _cupsStrFree(pc->product);
  cupsArrayDelete(pc->filters);
  cupsArrayDelete(pc->prefilters);
  cupsArrayDelete(pc->finishings);

  _cupsStrFree(pc->charge_info_uri);
  _cupsStrFree(pc->password);

  cupsArrayDelete(pc->mandatory);

  cupsArrayDelete(pc->support_files);

  _cupsStrFree(pc->cups_3d);
  _cupsStrFree(pc->cups_layer_order);

  cupsArrayDelete(pc->materials);

  free(pc);
}