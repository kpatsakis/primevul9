_ppdCacheCreateWithFile(
    const char *filename,		/* I  - File to read */
    ipp_t      **attrs)			/* IO - IPP attributes, if any */
{
  cups_file_t	*fp;			/* File */
  _ppd_cache_t	*pc;			/* PWG mapping data */
  pwg_size_t	*size;			/* Current size */
  pwg_map_t	*map;			/* Current map */
  _pwg_finishings_t *finishings;	/* Current finishings option */
  int		linenum,		/* Current line number */
		num_bins,		/* Number of bins in file */
		num_sizes,		/* Number of sizes in file */
		num_sources,		/* Number of sources in file */
		num_types;		/* Number of types in file */
  char		line[2048],		/* Current line */
		*value,			/* Pointer to value in line */
		*valueptr,		/* Pointer into value */
		pwg_keyword[128],	/* PWG keyword */
		ppd_keyword[PPD_MAX_NAME];
					/* PPD keyword */
  _pwg_print_color_mode_t print_color_mode;
					/* Print color mode for preset */
  _pwg_print_quality_t print_quality;	/* Print quality for preset */


  DEBUG_printf(("_ppdCacheCreateWithFile(filename=\"%s\")", filename));

 /*
  * Range check input...
  */

  if (attrs)
    *attrs = NULL;

  if (!filename)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(EINVAL), 0);
    return (NULL);
  }

 /*
  * Open the file...
  */

  if ((fp = cupsFileOpen(filename, "r")) == NULL)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
    return (NULL);
  }

 /*
  * Read the first line and make sure it has "#CUPS-PPD-CACHE-version" in it...
  */

  if (!cupsFileGets(fp, line, sizeof(line)))
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
    DEBUG_puts("_ppdCacheCreateWithFile: Unable to read first line.");
    cupsFileClose(fp);
    return (NULL);
  }

  if (strncmp(line, "#CUPS-PPD-CACHE-", 16))
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
    DEBUG_printf(("_ppdCacheCreateWithFile: Wrong first line \"%s\".", line));
    cupsFileClose(fp);
    return (NULL);
  }

  if (atoi(line + 16) != _PPD_CACHE_VERSION)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Out of date PPD cache file."), 1);
    DEBUG_printf(("_ppdCacheCreateWithFile: Cache file has version %s, "
                  "expected %d.", line + 16, _PPD_CACHE_VERSION));
    cupsFileClose(fp);
    return (NULL);
  }

 /*
  * Allocate the mapping data structure...
  */

  if ((pc = calloc(1, sizeof(_ppd_cache_t))) == NULL)
  {
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
    DEBUG_puts("_ppdCacheCreateWithFile: Unable to allocate _ppd_cache_t.");
    goto create_error;
  }

  pc->max_copies = 9999;

 /*
  * Read the file...
  */

  linenum     = 0;
  num_bins    = 0;
  num_sizes   = 0;
  num_sources = 0;
  num_types   = 0;

  while (cupsFileGetConf(fp, line, sizeof(line), &value, &linenum))
  {
    DEBUG_printf(("_ppdCacheCreateWithFile: line=\"%s\", value=\"%s\", "
                  "linenum=%d", line, value, linenum));

    if (!value)
    {
      DEBUG_printf(("_ppdCacheCreateWithFile: Missing value on line %d.",
                    linenum));
      _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
      goto create_error;
    }
    else if (!_cups_strcasecmp(line, "3D"))
    {
      pc->cups_3d = _cupsStrAlloc(value);
    }
    else if (!_cups_strcasecmp(line, "LayerOrder"))
    {
      pc->cups_layer_order = _cupsStrAlloc(value);
    }
    else if (!_cups_strcasecmp(line, "Accuracy"))
    {
      sscanf(value, "%d%d%d", pc->cups_accuracy + 0, pc->cups_accuracy + 1, pc->cups_accuracy + 2);
    }
    else if (!_cups_strcasecmp(line, "Volume"))
    {
      sscanf(value, "%d%d%d", pc->cups_volume + 0, pc->cups_volume + 1, pc->cups_volume + 2);
    }
    else if (!_cups_strcasecmp(line, "Material"))
    {
     /*
      * Material key "name" name=value ... name=value
      */

      if ((valueptr = strchr(value, ' ')) != NULL)
      {
	_pwg_material_t	*material = (_pwg_material_t *)calloc(1, sizeof(_pwg_material_t));

        *valueptr++ = '\0';

        material->key = _cupsStrAlloc(value);

        if (*valueptr == '\"')
	{
	  value = valueptr + 1;
	  if ((valueptr = strchr(value, '\"')) != NULL)
	  {
	    *valueptr++ = '\0';
	    material->name = _cupsStrAlloc(value);
	    material->num_props = cupsParseOptions(valueptr, 0, &material->props);
	  }
	}

	if (!pc->materials)
	  pc->materials = cupsArrayNew3(NULL, NULL, NULL, 0, NULL, (cups_afree_func_t)pwg_free_material);

        cupsArrayAdd(pc->materials, material);
      }
    }
    else if (!_cups_strcasecmp(line, "Filter"))
    {
      if (!pc->filters)
        pc->filters = cupsArrayNew3(NULL, NULL, NULL, 0,
	                            (cups_acopy_func_t)_cupsStrAlloc,
				    (cups_afree_func_t)_cupsStrFree);

      cupsArrayAdd(pc->filters, value);
    }
    else if (!_cups_strcasecmp(line, "PreFilter"))
    {
      if (!pc->prefilters)
        pc->prefilters = cupsArrayNew3(NULL, NULL, NULL, 0,
	                               (cups_acopy_func_t)_cupsStrAlloc,
				       (cups_afree_func_t)_cupsStrFree);

      cupsArrayAdd(pc->prefilters, value);
    }
    else if (!_cups_strcasecmp(line, "Product"))
    {
      pc->product = _cupsStrAlloc(value);
    }
    else if (!_cups_strcasecmp(line, "SingleFile"))
    {
      pc->single_file = !_cups_strcasecmp(value, "true");
    }
    else if (!_cups_strcasecmp(line, "IPP"))
    {
      off_t	pos = cupsFileTell(fp),	/* Position in file */
		length = strtol(value, NULL, 10);
					/* Length of IPP attributes */

      if (attrs && *attrs)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: IPP listed multiple times.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }
      else if (length <= 0)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: Bad IPP length.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (attrs)
      {
       /*
        * Read IPP attributes into the provided variable...
	*/

        *attrs = ippNew();

        if (ippReadIO(fp, (ipp_iocb_t)cupsFileRead, 1, NULL,
		      *attrs) != IPP_STATE_DATA)
	{
	  DEBUG_puts("_ppdCacheCreateWithFile: Bad IPP data.");
	  _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	  goto create_error;
	}
      }
      else
      {
       /*
        * Skip the IPP data entirely...
	*/

        cupsFileSeek(fp, pos + length);
      }

      if (cupsFileTell(fp) != (pos + length))
      {
        DEBUG_puts("_ppdCacheCreateWithFile: Bad IPP data.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }
    }
    else if (!_cups_strcasecmp(line, "NumBins"))
    {
      if (num_bins > 0)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: NumBins listed multiple times.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((num_bins = atoi(value)) <= 0 || num_bins > 65536)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad NumBins value %d on line "
		      "%d.", num_sizes, linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((pc->bins = calloc((size_t)num_bins, sizeof(pwg_map_t))) == NULL)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Unable to allocate %d bins.",
	              num_sizes));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
	goto create_error;
      }
    }
    else if (!_cups_strcasecmp(line, "Bin"))
    {
      if (sscanf(value, "%127s%40s", pwg_keyword, ppd_keyword) != 2)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad Bin on line %d.", linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (pc->num_bins >= num_bins)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Too many Bin's on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      map      = pc->bins + pc->num_bins;
      map->pwg = _cupsStrAlloc(pwg_keyword);
      map->ppd = _cupsStrAlloc(ppd_keyword);

      pc->num_bins ++;
    }
    else if (!_cups_strcasecmp(line, "NumSizes"))
    {
      if (num_sizes > 0)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: NumSizes listed multiple times.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((num_sizes = atoi(value)) < 0 || num_sizes > 65536)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad NumSizes value %d on line "
	              "%d.", num_sizes, linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (num_sizes > 0)
      {
	if ((pc->sizes = calloc((size_t)num_sizes, sizeof(pwg_size_t))) == NULL)
	{
	  DEBUG_printf(("_ppdCacheCreateWithFile: Unable to allocate %d sizes.",
			num_sizes));
	  _cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
	  goto create_error;
	}
      }
    }
    else if (!_cups_strcasecmp(line, "Size"))
    {
      if (pc->num_sizes >= num_sizes)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Too many Size's on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      size = pc->sizes + pc->num_sizes;

      if (sscanf(value, "%127s%40s%d%d%d%d%d%d", pwg_keyword, ppd_keyword,
		 &(size->width), &(size->length), &(size->left),
		 &(size->bottom), &(size->right), &(size->top)) != 8)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad Size on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      size->map.pwg = _cupsStrAlloc(pwg_keyword);
      size->map.ppd = _cupsStrAlloc(ppd_keyword);

      pc->num_sizes ++;
    }
    else if (!_cups_strcasecmp(line, "CustomSize"))
    {
      if (pc->custom_max_width > 0)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Too many CustomSize's on line "
	              "%d.", linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (sscanf(value, "%d%d%d%d%d%d%d%d", &(pc->custom_max_width),
                 &(pc->custom_max_length), &(pc->custom_min_width),
		 &(pc->custom_min_length), &(pc->custom_size.left),
		 &(pc->custom_size.bottom), &(pc->custom_size.right),
		 &(pc->custom_size.top)) != 8)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad CustomSize on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      pwgFormatSizeName(pwg_keyword, sizeof(pwg_keyword), "custom", "max",
		        pc->custom_max_width, pc->custom_max_length, NULL);
      pc->custom_max_keyword = _cupsStrAlloc(pwg_keyword);

      pwgFormatSizeName(pwg_keyword, sizeof(pwg_keyword), "custom", "min",
		        pc->custom_min_width, pc->custom_min_length, NULL);
      pc->custom_min_keyword = _cupsStrAlloc(pwg_keyword);
    }
    else if (!_cups_strcasecmp(line, "SourceOption"))
    {
      pc->source_option = _cupsStrAlloc(value);
    }
    else if (!_cups_strcasecmp(line, "NumSources"))
    {
      if (num_sources > 0)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: NumSources listed multiple "
	           "times.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((num_sources = atoi(value)) <= 0 || num_sources > 65536)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad NumSources value %d on "
	              "line %d.", num_sources, linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((pc->sources = calloc((size_t)num_sources, sizeof(pwg_map_t))) == NULL)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Unable to allocate %d sources.",
	              num_sources));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
	goto create_error;
      }
    }
    else if (!_cups_strcasecmp(line, "Source"))
    {
      if (sscanf(value, "%127s%40s", pwg_keyword, ppd_keyword) != 2)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad Source on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (pc->num_sources >= num_sources)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Too many Source's on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      map      = pc->sources + pc->num_sources;
      map->pwg = _cupsStrAlloc(pwg_keyword);
      map->ppd = _cupsStrAlloc(ppd_keyword);

      pc->num_sources ++;
    }
    else if (!_cups_strcasecmp(line, "NumTypes"))
    {
      if (num_types > 0)
      {
        DEBUG_puts("_ppdCacheCreateWithFile: NumTypes listed multiple times.");
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((num_types = atoi(value)) <= 0 || num_types > 65536)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad NumTypes value %d on "
	              "line %d.", num_types, linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if ((pc->types = calloc((size_t)num_types, sizeof(pwg_map_t))) == NULL)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Unable to allocate %d types.",
	              num_types));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, strerror(errno), 0);
	goto create_error;
      }
    }
    else if (!_cups_strcasecmp(line, "Type"))
    {
      if (sscanf(value, "%127s%40s", pwg_keyword, ppd_keyword) != 2)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad Type on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      if (pc->num_types >= num_types)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Too many Type's on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      map      = pc->types + pc->num_types;
      map->pwg = _cupsStrAlloc(pwg_keyword);
      map->ppd = _cupsStrAlloc(ppd_keyword);

      pc->num_types ++;
    }
    else if (!_cups_strcasecmp(line, "Preset"))
    {
     /*
      * Preset output-mode print-quality name=value ...
      */

      print_color_mode = (_pwg_print_color_mode_t)strtol(value, &valueptr, 10);
      print_quality    = (_pwg_print_quality_t)strtol(valueptr, &valueptr, 10);

      if (print_color_mode < _PWG_PRINT_COLOR_MODE_MONOCHROME ||
          print_color_mode >= _PWG_PRINT_COLOR_MODE_MAX ||
	  print_quality < _PWG_PRINT_QUALITY_DRAFT ||
	  print_quality >= _PWG_PRINT_QUALITY_MAX ||
	  valueptr == value || !*valueptr)
      {
        DEBUG_printf(("_ppdCacheCreateWithFile: Bad Preset on line %d.",
	              linenum));
	_cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
	goto create_error;
      }

      pc->num_presets[print_color_mode][print_quality] =
          cupsParseOptions(valueptr, 0,
	                   pc->presets[print_color_mode] + print_quality);
    }
    else if (!_cups_strcasecmp(line, "SidesOption"))
      pc->sides_option = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "Sides1Sided"))
      pc->sides_1sided = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "Sides2SidedLong"))
      pc->sides_2sided_long = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "Sides2SidedShort"))
      pc->sides_2sided_short = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "Finishings"))
    {
      if (!pc->finishings)
	pc->finishings =
	    cupsArrayNew3((cups_array_func_t)pwg_compare_finishings,
			  NULL, NULL, 0, NULL,
			  (cups_afree_func_t)pwg_free_finishings);

      if ((finishings = calloc(1, sizeof(_pwg_finishings_t))) == NULL)
        goto create_error;

      finishings->value       = (ipp_finishings_t)strtol(value, &valueptr, 10);
      finishings->num_options = cupsParseOptions(valueptr, 0,
                                                 &(finishings->options));

      cupsArrayAdd(pc->finishings, finishings);
    }
    else if (!_cups_strcasecmp(line, "MaxCopies"))
      pc->max_copies = atoi(value);
    else if (!_cups_strcasecmp(line, "ChargeInfoURI"))
      pc->charge_info_uri = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "JobAccountId"))
      pc->account_id = !_cups_strcasecmp(value, "true");
    else if (!_cups_strcasecmp(line, "JobAccountingUserId"))
      pc->accounting_user_id = !_cups_strcasecmp(value, "true");
    else if (!_cups_strcasecmp(line, "JobPassword"))
      pc->password = _cupsStrAlloc(value);
    else if (!_cups_strcasecmp(line, "Mandatory"))
    {
      if (pc->mandatory)
        _cupsArrayAddStrings(pc->mandatory, value, ' ');
      else
        pc->mandatory = _cupsArrayNewStrings(value, ' ');
    }
    else if (!_cups_strcasecmp(line, "SupportFile"))
    {
      if (!pc->support_files)
        pc->support_files = cupsArrayNew3(NULL, NULL, NULL, 0,
                                          (cups_acopy_func_t)_cupsStrAlloc,
                                          (cups_afree_func_t)_cupsStrFree);

      cupsArrayAdd(pc->support_files, value);
    }
    else
    {
      DEBUG_printf(("_ppdCacheCreateWithFile: Unknown %s on line %d.", line,
		    linenum));
    }
  }

  if (pc->num_sizes < num_sizes)
  {
    DEBUG_printf(("_ppdCacheCreateWithFile: Not enough sizes (%d < %d).",
                  pc->num_sizes, num_sizes));
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
    goto create_error;
  }

  if (pc->num_sources < num_sources)
  {
    DEBUG_printf(("_ppdCacheCreateWithFile: Not enough sources (%d < %d).",
                  pc->num_sources, num_sources));
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
    goto create_error;
  }

  if (pc->num_types < num_types)
  {
    DEBUG_printf(("_ppdCacheCreateWithFile: Not enough types (%d < %d).",
                  pc->num_types, num_types));
    _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Bad PPD cache file."), 1);
    goto create_error;
  }

  cupsFileClose(fp);

  return (pc);

 /*
  * If we get here the file was bad - free any data and return...
  */

  create_error:

  cupsFileClose(fp);
  _ppdCacheDestroy(pc);

  if (attrs)
  {
    ippDelete(*attrs);
    *attrs = NULL;
  }

  return (NULL);
}