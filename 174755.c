_ppdCacheWriteFile(
    _ppd_cache_t *pc,			/* I - PPD cache and mapping data */
    const char   *filename,		/* I - File to write */
    ipp_t        *attrs)		/* I - Attributes to write, if any */
{
  int			i, j, k;	/* Looping vars */
  cups_file_t		*fp;		/* Output file */
  pwg_size_t		*size;		/* Current size */
  pwg_map_t		*map;		/* Current map */
  _pwg_finishings_t	*f;		/* Current finishing option */
  cups_option_t		*option;	/* Current option */
  const char		*value;		/* Filter/pre-filter value */
  char			newfile[1024];	/* New filename */
  _pwg_material_t	*m;		/* Material */


 /*
  * Range check input...
  */

  if (!pc || !filename)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(EINVAL), 0);
    return (0);
  }

 /*
  * Open the file and write with compression...
  */

  snprintf(newfile, sizeof(newfile), "%s.N", filename);
  if ((fp = cupsFileOpen(newfile, "w9")) == NULL)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
    return (0);
  }

 /*
  * Standard header...
  */

  cupsFilePrintf(fp, "#CUPS-PPD-CACHE-%d\n", _PPD_CACHE_VERSION);

 /*
  * Output bins...
  */

  if (pc->num_bins > 0)
  {
    cupsFilePrintf(fp, "NumBins %d\n", pc->num_bins);
    for (i = pc->num_bins, map = pc->bins; i > 0; i --, map ++)
      cupsFilePrintf(fp, "Bin %s %s\n", map->pwg, map->ppd);
  }

 /*
  * Media sizes...
  */

  cupsFilePrintf(fp, "NumSizes %d\n", pc->num_sizes);
  for (i = pc->num_sizes, size = pc->sizes; i > 0; i --, size ++)
    cupsFilePrintf(fp, "Size %s %s %d %d %d %d %d %d\n", size->map.pwg,
		   size->map.ppd, size->width, size->length, size->left,
		   size->bottom, size->right, size->top);
  if (pc->custom_max_width > 0)
    cupsFilePrintf(fp, "CustomSize %d %d %d %d %d %d %d %d\n",
                   pc->custom_max_width, pc->custom_max_length,
		   pc->custom_min_width, pc->custom_min_length,
		   pc->custom_size.left, pc->custom_size.bottom,
		   pc->custom_size.right, pc->custom_size.top);

 /*
  * Media sources...
  */

  if (pc->source_option)
    cupsFilePrintf(fp, "SourceOption %s\n", pc->source_option);

  if (pc->num_sources > 0)
  {
    cupsFilePrintf(fp, "NumSources %d\n", pc->num_sources);
    for (i = pc->num_sources, map = pc->sources; i > 0; i --, map ++)
      cupsFilePrintf(fp, "Source %s %s\n", map->pwg, map->ppd);
  }

 /*
  * Media types...
  */

  if (pc->num_types > 0)
  {
    cupsFilePrintf(fp, "NumTypes %d\n", pc->num_types);
    for (i = pc->num_types, map = pc->types; i > 0; i --, map ++)
      cupsFilePrintf(fp, "Type %s %s\n", map->pwg, map->ppd);
  }

 /*
  * Presets...
  */

  for (i = _PWG_PRINT_COLOR_MODE_MONOCHROME; i < _PWG_PRINT_COLOR_MODE_MAX; i ++)
    for (j = _PWG_PRINT_QUALITY_DRAFT; j < _PWG_PRINT_QUALITY_MAX; j ++)
      if (pc->num_presets[i][j])
      {
	cupsFilePrintf(fp, "Preset %d %d", i, j);
	for (k = pc->num_presets[i][j], option = pc->presets[i][j];
	     k > 0;
	     k --, option ++)
	  cupsFilePrintf(fp, " %s=%s", option->name, option->value);
	cupsFilePutChar(fp, '\n');
      }

 /*
  * Duplex/sides...
  */

  if (pc->sides_option)
    cupsFilePrintf(fp, "SidesOption %s\n", pc->sides_option);

  if (pc->sides_1sided)
    cupsFilePrintf(fp, "Sides1Sided %s\n", pc->sides_1sided);

  if (pc->sides_2sided_long)
    cupsFilePrintf(fp, "Sides2SidedLong %s\n", pc->sides_2sided_long);

  if (pc->sides_2sided_short)
    cupsFilePrintf(fp, "Sides2SidedShort %s\n", pc->sides_2sided_short);

 /*
  * Product, cupsFilter, cupsFilter2, and cupsPreFilter...
  */

  if (pc->product)
    cupsFilePutConf(fp, "Product", pc->product);

  for (value = (const char *)cupsArrayFirst(pc->filters);
       value;
       value = (const char *)cupsArrayNext(pc->filters))
    cupsFilePutConf(fp, "Filter", value);

  for (value = (const char *)cupsArrayFirst(pc->prefilters);
       value;
       value = (const char *)cupsArrayNext(pc->prefilters))
    cupsFilePutConf(fp, "PreFilter", value);

  cupsFilePrintf(fp, "SingleFile %s\n", pc->single_file ? "true" : "false");

 /*
  * Finishing options...
  */

  for (f = (_pwg_finishings_t *)cupsArrayFirst(pc->finishings);
       f;
       f = (_pwg_finishings_t *)cupsArrayNext(pc->finishings))
  {
    cupsFilePrintf(fp, "Finishings %d", f->value);
    for (i = f->num_options, option = f->options; i > 0; i --, option ++)
      cupsFilePrintf(fp, " %s=%s", option->name, option->value);
    cupsFilePutChar(fp, '\n');
  }

 /*
  * Max copies...
  */

  cupsFilePrintf(fp, "MaxCopies %d\n", pc->max_copies);

 /*
  * Accounting/quota/PIN/managed printing values...
  */

  if (pc->charge_info_uri)
    cupsFilePutConf(fp, "ChargeInfoURI", pc->charge_info_uri);

  cupsFilePrintf(fp, "AccountId %s\n", pc->account_id ? "true" : "false");
  cupsFilePrintf(fp, "AccountingUserId %s\n",
                 pc->accounting_user_id ? "true" : "false");

  if (pc->password)
    cupsFilePutConf(fp, "Password", pc->password);

  for (value = (char *)cupsArrayFirst(pc->mandatory);
       value;
       value = (char *)cupsArrayNext(pc->mandatory))
    cupsFilePutConf(fp, "Mandatory", value);

 /*
  * Support files...
  */

  for (value = (char *)cupsArrayFirst(pc->support_files);
       value;
       value = (char *)cupsArrayNext(pc->support_files))
    cupsFilePutConf(fp, "SupportFile", value);

 /*
  * 3D stuff...
  */

  if (pc->cups_3d)
    cupsFilePutConf(fp, "3D", pc->cups_3d);

  if (pc->cups_layer_order)
    cupsFilePutConf(fp, "LayerOrder", pc->cups_layer_order);

  if (pc->cups_accuracy[0] || pc->cups_accuracy[0] || pc->cups_accuracy[2])
    cupsFilePrintf(fp, "Accuracy %d %d %d\n", pc->cups_accuracy[0], pc->cups_accuracy[1], pc->cups_accuracy[2]);

  if (pc->cups_volume[0] || pc->cups_volume[0] || pc->cups_volume[2])
    cupsFilePrintf(fp, "Volume %d %d %d\n", pc->cups_volume[0], pc->cups_volume[1], pc->cups_volume[2]);

  for (m = (_pwg_material_t *)cupsArrayFirst(pc->materials);
       m;
       m = (_pwg_material_t *)cupsArrayNext(pc->materials))
  {
    cupsFilePrintf(fp, "Material %s \"%s\"", m->key, m->name);
    for (i = 0; i < m->num_props; i ++)
      cupsFilePrintf(fp, " %s=%s", m->props[i].name, m->props[i].value);
    cupsFilePuts(fp, "\n");
  }

 /*
  * IPP attributes, if any...
  */

  if (attrs)
  {
    cupsFilePrintf(fp, "IPP " CUPS_LLFMT "\n", CUPS_LLCAST ippLength(attrs));

    attrs->state = IPP_STATE_IDLE;
    ippWriteIO(fp, (ipp_iocb_t)cupsFileWrite, 1, NULL, attrs);
  }

 /*
  * Close and return...
  */

  if (cupsFileClose(fp))
  {
    unlink(newfile);
    return (0);
  }

  unlink(filename);
  return (!rename(newfile, filename));
}