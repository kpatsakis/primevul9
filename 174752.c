_ppdCacheCreateWithPPD(ppd_file_t *ppd)	/* I - PPD file */
{
  int			i, j, k;	/* Looping vars */
  _ppd_cache_t		*pc;		/* PWG mapping data */
  ppd_option_t		*input_slot,	/* InputSlot option */
			*media_type,	/* MediaType option */
			*output_bin,	/* OutputBin option */
			*color_model,	/* ColorModel option */
			*duplex;	/* Duplex option */
  ppd_choice_t		*choice;	/* Current InputSlot/MediaType */
  pwg_map_t		*map;		/* Current source/type map */
  ppd_attr_t		*ppd_attr;	/* Current PPD preset attribute */
  int			num_options;	/* Number of preset options and props */
  cups_option_t		*options;	/* Preset options and properties */
  ppd_size_t		*ppd_size;	/* Current PPD size */
  pwg_size_t		*pwg_size;	/* Current PWG size */
  char			pwg_keyword[3 + PPD_MAX_NAME + 1 + 12 + 1 + 12 + 3],
					/* PWG keyword string */
			ppd_name[PPD_MAX_NAME];
					/* Normalized PPD name */
  const char		*pwg_name;	/* Standard PWG media name */
  pwg_media_t		*pwg_media;	/* PWG media data */
  _pwg_print_color_mode_t pwg_print_color_mode;
					/* print-color-mode index */
  _pwg_print_quality_t	pwg_print_quality;
					/* print-quality index */
  int			similar;	/* Are the old and new size similar? */
  pwg_size_t		*old_size;	/* Current old size */
  int			old_imageable,	/* Old imageable length in 2540ths */
			old_borderless,	/* Old borderless state */
			old_known_pwg;	/* Old PWG name is well-known */
  int			new_width,	/* New width in 2540ths */
			new_length,	/* New length in 2540ths */
			new_left,	/* New left margin in 2540ths */
			new_bottom,	/* New bottom margin in 2540ths */
			new_right,	/* New right margin in 2540ths */
			new_top,	/* New top margin in 2540ths */
			new_imageable,	/* New imageable length in 2540ths */
			new_borderless,	/* New borderless state */
			new_known_pwg;	/* New PWG name is well-known */
  pwg_size_t		*new_size;	/* New size to add, if any */
  const char		*filter;	/* Current filter */
  _pwg_finishings_t	*finishings;	/* Current finishings value */


  DEBUG_printf(("_ppdCacheCreateWithPPD(ppd=%p)", ppd));

 /*
  * Range check input...
  */

  if (!ppd)
    return (NULL);

 /*
  * Allocate memory...
  */

  if ((pc = calloc(1, sizeof(_ppd_cache_t))) == NULL)
  {
    DEBUG_puts("_ppdCacheCreateWithPPD: Unable to allocate _ppd_cache_t.");
    goto create_error;
  }

 /*
  * Copy and convert size data...
  */

  if (ppd->num_sizes > 0)
  {
    if ((pc->sizes = calloc((size_t)ppd->num_sizes, sizeof(pwg_size_t))) == NULL)
    {
      DEBUG_printf(("_ppdCacheCreateWithPPD: Unable to allocate %d "
		    "pwg_size_t's.", ppd->num_sizes));
      goto create_error;
    }

    for (i = ppd->num_sizes, pwg_size = pc->sizes, ppd_size = ppd->sizes;
	 i > 0;
	 i --, ppd_size ++)
    {
     /*
      * Don't copy over custom size...
      */

      if (!_cups_strcasecmp(ppd_size->name, "Custom"))
	continue;

     /*
      * Convert the PPD size name to the corresponding PWG keyword name.
      */

      if ((pwg_media = pwgMediaForPPD(ppd_size->name)) != NULL)
      {
       /*
	* Standard name, do we have conflicts?
	*/

	for (j = 0; j < pc->num_sizes; j ++)
	  if (!strcmp(pc->sizes[j].map.pwg, pwg_media->pwg))
	  {
	    pwg_media = NULL;
	    break;
	  }
      }

      if (pwg_media)
      {
       /*
	* Standard name and no conflicts, use it!
	*/

	pwg_name      = pwg_media->pwg;
	new_known_pwg = 1;
      }
      else
      {
       /*
	* Not a standard name; convert it to a PWG vendor name of the form:
	*
	*     pp_lowerppd_WIDTHxHEIGHTuu
	*/

	pwg_name      = pwg_keyword;
	new_known_pwg = 0;

	pwg_unppdize_name(ppd_size->name, ppd_name, sizeof(ppd_name), "_.");
	pwgFormatSizeName(pwg_keyword, sizeof(pwg_keyword), NULL, ppd_name,
			  PWG_FROM_POINTS(ppd_size->width),
			  PWG_FROM_POINTS(ppd_size->length), NULL);
      }

     /*
      * If we have a similar paper with non-zero margins then we only want to
      * keep it if it has a larger imageable area length.  The NULL check is for
      * dimensions that are <= 0...
      */

      if ((pwg_media = _pwgMediaNearSize(PWG_FROM_POINTS(ppd_size->width),
					PWG_FROM_POINTS(ppd_size->length),
					0)) == NULL)
	continue;

      new_width      = pwg_media->width;
      new_length     = pwg_media->length;
      new_left       = PWG_FROM_POINTS(ppd_size->left);
      new_bottom     = PWG_FROM_POINTS(ppd_size->bottom);
      new_right      = PWG_FROM_POINTS(ppd_size->width - ppd_size->right);
      new_top        = PWG_FROM_POINTS(ppd_size->length - ppd_size->top);
      new_imageable  = new_length - new_top - new_bottom;
      new_borderless = new_bottom == 0 && new_top == 0 &&
		       new_left == 0 && new_right == 0;

      for (k = pc->num_sizes, similar = 0, old_size = pc->sizes, new_size = NULL;
	   k > 0 && !similar;
	   k --, old_size ++)
      {
	old_imageable  = old_size->length - old_size->top - old_size->bottom;
	old_borderless = old_size->left == 0 && old_size->bottom == 0 &&
			 old_size->right == 0 && old_size->top == 0;
	old_known_pwg  = strncmp(old_size->map.pwg, "oe_", 3) &&
			 strncmp(old_size->map.pwg, "om_", 3);

	similar = old_borderless == new_borderless &&
		  _PWG_EQUIVALENT(old_size->width, new_width) &&
		  _PWG_EQUIVALENT(old_size->length, new_length);

	if (similar &&
	    (new_known_pwg || (!old_known_pwg && new_imageable > old_imageable)))
	{
	 /*
	  * The new paper has a larger imageable area so it could replace
	  * the older paper.  Regardless of the imageable area, we always
	  * prefer the size with a well-known PWG name.
	  */

	  new_size = old_size;
	  _cupsStrFree(old_size->map.ppd);
	  _cupsStrFree(old_size->map.pwg);
	}
      }

      if (!similar)
      {
       /*
	* The paper was unique enough to deserve its own entry so add it to the
	* end.
	*/

	new_size = pwg_size ++;
	pc->num_sizes ++;
      }

      if (new_size)
      {
       /*
	* Save this size...
	*/

	new_size->map.ppd = _cupsStrAlloc(ppd_size->name);
	new_size->map.pwg = _cupsStrAlloc(pwg_name);
	new_size->width   = new_width;
	new_size->length  = new_length;
	new_size->left    = new_left;
	new_size->bottom  = new_bottom;
	new_size->right   = new_right;
	new_size->top     = new_top;
      }
    }
  }

  if (ppd->variable_sizes)
  {
   /*
    * Generate custom size data...
    */

    pwgFormatSizeName(pwg_keyword, sizeof(pwg_keyword), "custom", "max",
		      PWG_FROM_POINTS(ppd->custom_max[0]),
		      PWG_FROM_POINTS(ppd->custom_max[1]), NULL);
    pc->custom_max_keyword = _cupsStrAlloc(pwg_keyword);
    pc->custom_max_width   = PWG_FROM_POINTS(ppd->custom_max[0]);
    pc->custom_max_length  = PWG_FROM_POINTS(ppd->custom_max[1]);

    pwgFormatSizeName(pwg_keyword, sizeof(pwg_keyword), "custom", "min",
		      PWG_FROM_POINTS(ppd->custom_min[0]),
		      PWG_FROM_POINTS(ppd->custom_min[1]), NULL);
    pc->custom_min_keyword = _cupsStrAlloc(pwg_keyword);
    pc->custom_min_width   = PWG_FROM_POINTS(ppd->custom_min[0]);
    pc->custom_min_length  = PWG_FROM_POINTS(ppd->custom_min[1]);

    pc->custom_size.left   = PWG_FROM_POINTS(ppd->custom_margins[0]);
    pc->custom_size.bottom = PWG_FROM_POINTS(ppd->custom_margins[1]);
    pc->custom_size.right  = PWG_FROM_POINTS(ppd->custom_margins[2]);
    pc->custom_size.top    = PWG_FROM_POINTS(ppd->custom_margins[3]);
  }

 /*
  * Copy and convert InputSlot data...
  */

  if ((input_slot = ppdFindOption(ppd, "InputSlot")) == NULL)
    input_slot = ppdFindOption(ppd, "HPPaperSource");

  if (input_slot)
  {
    pc->source_option = _cupsStrAlloc(input_slot->keyword);

    if ((pc->sources = calloc((size_t)input_slot->num_choices, sizeof(pwg_map_t))) == NULL)
    {
      DEBUG_printf(("_ppdCacheCreateWithPPD: Unable to allocate %d "
                    "pwg_map_t's for InputSlot.", input_slot->num_choices));
      goto create_error;
    }

    pc->num_sources = input_slot->num_choices;

    for (i = input_slot->num_choices, choice = input_slot->choices,
             map = pc->sources;
	 i > 0;
	 i --, choice ++, map ++)
    {
      if (!_cups_strncasecmp(choice->choice, "Auto", 4) ||
          !_cups_strcasecmp(choice->choice, "Default"))
        pwg_name = "auto";
      else if (!_cups_strcasecmp(choice->choice, "Cassette"))
        pwg_name = "main";
      else if (!_cups_strcasecmp(choice->choice, "PhotoTray"))
        pwg_name = "photo";
      else if (!_cups_strcasecmp(choice->choice, "CDTray"))
        pwg_name = "disc";
      else if (!_cups_strncasecmp(choice->choice, "Multipurpose", 12) ||
               !_cups_strcasecmp(choice->choice, "MP") ||
               !_cups_strcasecmp(choice->choice, "MPTray"))
        pwg_name = "by-pass-tray";
      else if (!_cups_strcasecmp(choice->choice, "LargeCapacity"))
        pwg_name = "large-capacity";
      else if (!_cups_strncasecmp(choice->choice, "Lower", 5))
        pwg_name = "bottom";
      else if (!_cups_strncasecmp(choice->choice, "Middle", 6))
        pwg_name = "middle";
      else if (!_cups_strncasecmp(choice->choice, "Upper", 5))
        pwg_name = "top";
      else if (!_cups_strncasecmp(choice->choice, "Side", 4))
        pwg_name = "side";
      else if (!_cups_strcasecmp(choice->choice, "Roll"))
        pwg_name = "main-roll";
      else
      {
       /*
        * Convert PPD name to lowercase...
	*/

        pwg_name = pwg_keyword;
	pwg_unppdize_name(choice->choice, pwg_keyword, sizeof(pwg_keyword),
	                  "_");
      }

      map->pwg = _cupsStrAlloc(pwg_name);
      map->ppd = _cupsStrAlloc(choice->choice);
    }
  }

 /*
  * Copy and convert MediaType data...
  */

  if ((media_type = ppdFindOption(ppd, "MediaType")) != NULL)
  {
    if ((pc->types = calloc((size_t)media_type->num_choices, sizeof(pwg_map_t))) == NULL)
    {
      DEBUG_printf(("_ppdCacheCreateWithPPD: Unable to allocate %d "
                    "pwg_map_t's for MediaType.", media_type->num_choices));
      goto create_error;
    }

    pc->num_types = media_type->num_choices;

    for (i = media_type->num_choices, choice = media_type->choices,
             map = pc->types;
	 i > 0;
	 i --, choice ++, map ++)
    {
      if (!_cups_strncasecmp(choice->choice, "Auto", 4) ||
          !_cups_strcasecmp(choice->choice, "Any") ||
          !_cups_strcasecmp(choice->choice, "Default"))
        pwg_name = "auto";
      else if (!_cups_strncasecmp(choice->choice, "Card", 4))
        pwg_name = "cardstock";
      else if (!_cups_strncasecmp(choice->choice, "Env", 3))
        pwg_name = "envelope";
      else if (!_cups_strncasecmp(choice->choice, "Gloss", 5))
        pwg_name = "photographic-glossy";
      else if (!_cups_strcasecmp(choice->choice, "HighGloss"))
        pwg_name = "photographic-high-gloss";
      else if (!_cups_strcasecmp(choice->choice, "Matte"))
        pwg_name = "photographic-matte";
      else if (!_cups_strncasecmp(choice->choice, "Plain", 5))
        pwg_name = "stationery";
      else if (!_cups_strncasecmp(choice->choice, "Coated", 6))
        pwg_name = "stationery-coated";
      else if (!_cups_strcasecmp(choice->choice, "Inkjet"))
        pwg_name = "stationery-inkjet";
      else if (!_cups_strcasecmp(choice->choice, "Letterhead"))
        pwg_name = "stationery-letterhead";
      else if (!_cups_strncasecmp(choice->choice, "Preprint", 8))
        pwg_name = "stationery-preprinted";
      else if (!_cups_strcasecmp(choice->choice, "Recycled"))
        pwg_name = "stationery-recycled";
      else if (!_cups_strncasecmp(choice->choice, "Transparen", 10))
        pwg_name = "transparency";
      else
      {
       /*
        * Convert PPD name to lowercase...
	*/

        pwg_name = pwg_keyword;
	pwg_unppdize_name(choice->choice, pwg_keyword, sizeof(pwg_keyword),
	                  "_");
      }

      map->pwg = _cupsStrAlloc(pwg_name);
      map->ppd = _cupsStrAlloc(choice->choice);
    }
  }

 /*
  * Copy and convert OutputBin data...
  */

  if ((output_bin = ppdFindOption(ppd, "OutputBin")) != NULL)
  {
    if ((pc->bins = calloc((size_t)output_bin->num_choices, sizeof(pwg_map_t))) == NULL)
    {
      DEBUG_printf(("_ppdCacheCreateWithPPD: Unable to allocate %d "
                    "pwg_map_t's for OutputBin.", output_bin->num_choices));
      goto create_error;
    }

    pc->num_bins = output_bin->num_choices;

    for (i = output_bin->num_choices, choice = output_bin->choices,
             map = pc->bins;
	 i > 0;
	 i --, choice ++, map ++)
    {
      pwg_unppdize_name(choice->choice, pwg_keyword, sizeof(pwg_keyword), "_");

      map->pwg = _cupsStrAlloc(pwg_keyword);
      map->ppd = _cupsStrAlloc(choice->choice);
    }
  }

  if ((ppd_attr = ppdFindAttr(ppd, "APPrinterPreset", NULL)) != NULL)
  {
   /*
    * Copy and convert APPrinterPreset (output-mode + print-quality) data...
    */

    const char	*quality,		/* com.apple.print.preset.quality value */
		*output_mode,		/* com.apple.print.preset.output-mode value */
		*color_model_val,	/* ColorModel choice */
		*graphicsType,		/* com.apple.print.preset.graphicsType value */
		*media_front_coating;	/* com.apple.print.preset.media-front-coating value */

    do
    {
      num_options = _ppdParseOptions(ppd_attr->value, 0, &options,
                                     _PPD_PARSE_ALL);

      if ((quality = cupsGetOption("com.apple.print.preset.quality",
                                   num_options, options)) != NULL)
      {
       /*
        * Get the print-quality for this preset...
	*/

	if (!strcmp(quality, "low"))
	  pwg_print_quality = _PWG_PRINT_QUALITY_DRAFT;
	else if (!strcmp(quality, "high"))
	  pwg_print_quality = _PWG_PRINT_QUALITY_HIGH;
	else
	  pwg_print_quality = _PWG_PRINT_QUALITY_NORMAL;

       /*
	* Ignore graphicsType "Photo" presets that are not high quality.
	*/

	graphicsType = cupsGetOption("com.apple.print.preset.graphicsType",
				      num_options, options);

	if (pwg_print_quality != _PWG_PRINT_QUALITY_HIGH && graphicsType &&
	    !strcmp(graphicsType, "Photo"))
	  continue;

       /*
	* Ignore presets for normal and draft quality where the coating
	* isn't "none" or "autodetect".
	*/

	media_front_coating = cupsGetOption(
	                          "com.apple.print.preset.media-front-coating",
			          num_options, options);

        if (pwg_print_quality != _PWG_PRINT_QUALITY_HIGH &&
	    media_front_coating &&
	    strcmp(media_front_coating, "none") &&
	    strcmp(media_front_coating, "autodetect"))
	  continue;

       /*
        * Get the output mode for this preset...
	*/

        output_mode     = cupsGetOption("com.apple.print.preset.output-mode",
	                                num_options, options);
        color_model_val = cupsGetOption("ColorModel", num_options, options);

        if (output_mode)
	{
	  if (!strcmp(output_mode, "monochrome"))
	    pwg_print_color_mode = _PWG_PRINT_COLOR_MODE_MONOCHROME;
	  else
	    pwg_print_color_mode = _PWG_PRINT_COLOR_MODE_COLOR;
	}
	else if (color_model_val)
	{
	  if (!_cups_strcasecmp(color_model_val, "Gray"))
	    pwg_print_color_mode = _PWG_PRINT_COLOR_MODE_MONOCHROME;
	  else
	    pwg_print_color_mode = _PWG_PRINT_COLOR_MODE_COLOR;
	}
	else
	  pwg_print_color_mode = _PWG_PRINT_COLOR_MODE_COLOR;

       /*
        * Save the options for this combination as needed...
	*/

        if (!pc->num_presets[pwg_print_color_mode][pwg_print_quality])
	  pc->num_presets[pwg_print_color_mode][pwg_print_quality] =
	      _ppdParseOptions(ppd_attr->value, 0,
	                       pc->presets[pwg_print_color_mode] +
			           pwg_print_quality, _PPD_PARSE_OPTIONS);
      }

      cupsFreeOptions(num_options, options);
    }
    while ((ppd_attr = ppdFindNextAttr(ppd, "APPrinterPreset", NULL)) != NULL);
  }

  if (!pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][_PWG_PRINT_QUALITY_DRAFT] &&
      !pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][_PWG_PRINT_QUALITY_NORMAL] &&
      !pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][_PWG_PRINT_QUALITY_HIGH])
  {
   /*
    * Try adding some common color options to create grayscale presets.  These
    * are listed in order of popularity...
    */

    const char	*color_option = NULL,	/* Color control option */
		*gray_choice = NULL;	/* Choice to select grayscale */

    if ((color_model = ppdFindOption(ppd, "ColorModel")) != NULL &&
        ppdFindChoice(color_model, "Gray"))
    {
      color_option = "ColorModel";
      gray_choice  = "Gray";
    }
    else if ((color_model = ppdFindOption(ppd, "HPColorMode")) != NULL &&
             ppdFindChoice(color_model, "grayscale"))
    {
      color_option = "HPColorMode";
      gray_choice  = "grayscale";
    }
    else if ((color_model = ppdFindOption(ppd, "BRMonoColor")) != NULL &&
             ppdFindChoice(color_model, "Mono"))
    {
      color_option = "BRMonoColor";
      gray_choice  = "Mono";
    }
    else if ((color_model = ppdFindOption(ppd, "CNIJSGrayScale")) != NULL &&
             ppdFindChoice(color_model, "1"))
    {
      color_option = "CNIJSGrayScale";
      gray_choice  = "1";
    }
    else if ((color_model = ppdFindOption(ppd, "HPColorAsGray")) != NULL &&
             ppdFindChoice(color_model, "True"))
    {
      color_option = "HPColorAsGray";
      gray_choice  = "True";
    }

    if (color_option && gray_choice)
    {
     /*
      * Copy and convert ColorModel (output-mode) data...
      */

      cups_option_t	*coption,	/* Color option */
			  *moption;	/* Monochrome option */

      for (pwg_print_quality = _PWG_PRINT_QUALITY_DRAFT;
	   pwg_print_quality < _PWG_PRINT_QUALITY_MAX;
	   pwg_print_quality ++)
      {
	if (pc->num_presets[_PWG_PRINT_COLOR_MODE_COLOR][pwg_print_quality])
	{
	 /*
	  * Copy the color options...
	  */

	  num_options = pc->num_presets[_PWG_PRINT_COLOR_MODE_COLOR]
					[pwg_print_quality];
	  options     = calloc(sizeof(cups_option_t), (size_t)num_options);

	  if (options)
	  {
	    for (i = num_options, moption = options,
		     coption = pc->presets[_PWG_PRINT_COLOR_MODE_COLOR]
					   [pwg_print_quality];
		 i > 0;
		 i --, moption ++, coption ++)
	    {
	      moption->name  = _cupsStrRetain(coption->name);
	      moption->value = _cupsStrRetain(coption->value);
	    }

	    pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][pwg_print_quality] =
		num_options;
	    pc->presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][pwg_print_quality] =
		options;
	  }
	}
	else if (pwg_print_quality != _PWG_PRINT_QUALITY_NORMAL)
	  continue;

       /*
	* Add the grayscale option to the preset...
	*/

	pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME][pwg_print_quality] =
	    cupsAddOption(color_option, gray_choice,
			  pc->num_presets[_PWG_PRINT_COLOR_MODE_MONOCHROME]
					  [pwg_print_quality],
			  pc->presets[_PWG_PRINT_COLOR_MODE_MONOCHROME] +
			      pwg_print_quality);
      }
    }
  }

 /*
  * Copy and convert Duplex (sides) data...
  */

  if ((duplex = ppdFindOption(ppd, "Duplex")) == NULL)
    if ((duplex = ppdFindOption(ppd, "JCLDuplex")) == NULL)
      if ((duplex = ppdFindOption(ppd, "EFDuplex")) == NULL)
        if ((duplex = ppdFindOption(ppd, "EFDuplexing")) == NULL)
	  duplex = ppdFindOption(ppd, "KD03Duplex");

  if (duplex)
  {
    pc->sides_option = _cupsStrAlloc(duplex->keyword);

    for (i = duplex->num_choices, choice = duplex->choices;
         i > 0;
	 i --, choice ++)
    {
      if ((!_cups_strcasecmp(choice->choice, "None") ||
	   !_cups_strcasecmp(choice->choice, "False")) && !pc->sides_1sided)
        pc->sides_1sided = _cupsStrAlloc(choice->choice);
      else if ((!_cups_strcasecmp(choice->choice, "DuplexNoTumble") ||
	        !_cups_strcasecmp(choice->choice, "LongEdge") ||
	        !_cups_strcasecmp(choice->choice, "Top")) && !pc->sides_2sided_long)
        pc->sides_2sided_long = _cupsStrAlloc(choice->choice);
      else if ((!_cups_strcasecmp(choice->choice, "DuplexTumble") ||
	        !_cups_strcasecmp(choice->choice, "ShortEdge") ||
	        !_cups_strcasecmp(choice->choice, "Bottom")) &&
	       !pc->sides_2sided_short)
        pc->sides_2sided_short = _cupsStrAlloc(choice->choice);
    }
  }

 /*
  * Copy filters and pre-filters...
  */

  pc->filters = cupsArrayNew3(NULL, NULL, NULL, 0,
			      (cups_acopy_func_t)_cupsStrAlloc,
			      (cups_afree_func_t)_cupsStrFree);

  cupsArrayAdd(pc->filters,
               "application/vnd.cups-raw application/octet-stream 0 -");

  if ((ppd_attr = ppdFindAttr(ppd, "cupsFilter2", NULL)) != NULL)
  {
    do
    {
      cupsArrayAdd(pc->filters, ppd_attr->value);
    }
    while ((ppd_attr = ppdFindNextAttr(ppd, "cupsFilter2", NULL)) != NULL);
  }
  else if (ppd->num_filters > 0)
  {
    for (i = 0; i < ppd->num_filters; i ++)
      cupsArrayAdd(pc->filters, ppd->filters[i]);
  }
  else
    cupsArrayAdd(pc->filters, "application/vnd.cups-postscript 0 -");

 /*
  * See if we have a command filter...
  */

  for (filter = (const char *)cupsArrayFirst(pc->filters);
       filter;
       filter = (const char *)cupsArrayNext(pc->filters))
    if (!_cups_strncasecmp(filter, "application/vnd.cups-command", 28) &&
        _cups_isspace(filter[28]))
      break;

  if (!filter &&
      ((ppd_attr = ppdFindAttr(ppd, "cupsCommands", NULL)) == NULL ||
       _cups_strcasecmp(ppd_attr->value, "none")))
  {
   /*
    * No command filter and no cupsCommands keyword telling us not to use one.
    * See if this is a PostScript printer, and if so add a PostScript command
    * filter...
    */

    for (filter = (const char *)cupsArrayFirst(pc->filters);
	 filter;
	 filter = (const char *)cupsArrayNext(pc->filters))
      if (!_cups_strncasecmp(filter, "application/vnd.cups-postscript", 31) &&
	  _cups_isspace(filter[31]))
	break;

    if (filter)
      cupsArrayAdd(pc->filters,
                   "application/vnd.cups-command application/postscript 100 "
                   "commandtops");
  }

  if ((ppd_attr = ppdFindAttr(ppd, "cupsPreFilter", NULL)) != NULL)
  {
    pc->prefilters = cupsArrayNew3(NULL, NULL, NULL, 0,
				   (cups_acopy_func_t)_cupsStrAlloc,
				   (cups_afree_func_t)_cupsStrFree);

    do
    {
      cupsArrayAdd(pc->prefilters, ppd_attr->value);
    }
    while ((ppd_attr = ppdFindNextAttr(ppd, "cupsPreFilter", NULL)) != NULL);
  }

  if ((ppd_attr = ppdFindAttr(ppd, "cupsSingleFile", NULL)) != NULL)
    pc->single_file = !_cups_strcasecmp(ppd_attr->value, "true");

 /*
  * Copy the product string, if any...
  */

  if (ppd->product)
    pc->product = _cupsStrAlloc(ppd->product);

 /*
  * Copy finishings mapping data...
  */

  if ((ppd_attr = ppdFindAttr(ppd, "cupsIPPFinishings", NULL)) != NULL)
  {
    pc->finishings = cupsArrayNew3((cups_array_func_t)pwg_compare_finishings,
                                   NULL, NULL, 0, NULL,
                                   (cups_afree_func_t)pwg_free_finishings);

    do
    {
      if ((finishings = calloc(1, sizeof(_pwg_finishings_t))) == NULL)
        goto create_error;

      finishings->value       = (ipp_finishings_t)atoi(ppd_attr->spec);
      finishings->num_options = _ppdParseOptions(ppd_attr->value, 0,
                                                 &(finishings->options),
                                                 _PPD_PARSE_OPTIONS);

      cupsArrayAdd(pc->finishings, finishings);
    }
    while ((ppd_attr = ppdFindNextAttr(ppd, "cupsIPPFinishings",
                                       NULL)) != NULL);
  }

 /*
  * Max copies...
  */

  if ((ppd_attr = ppdFindAttr(ppd, "cupsMaxCopies", NULL)) != NULL)
    pc->max_copies = atoi(ppd_attr->value);
  else if (ppd->manual_copies)
    pc->max_copies = 1;
  else
    pc->max_copies = 9999;

 /*
  * cupsChargeInfoURI, cupsJobAccountId, cupsJobAccountingUserId,
  * cupsJobPassword, and cupsMandatory.
  */

  if ((ppd_attr = ppdFindAttr(ppd, "cupsChargeInfoURI", NULL)) != NULL)
    pc->charge_info_uri = _cupsStrAlloc(ppd_attr->value);

  if ((ppd_attr = ppdFindAttr(ppd, "cupsJobAccountId", NULL)) != NULL)
    pc->account_id = !_cups_strcasecmp(ppd_attr->value, "true");

  if ((ppd_attr = ppdFindAttr(ppd, "cupsJobAccountingUserId", NULL)) != NULL)
    pc->accounting_user_id = !_cups_strcasecmp(ppd_attr->value, "true");

  if ((ppd_attr = ppdFindAttr(ppd, "cupsJobPassword", NULL)) != NULL)
    pc->password = _cupsStrAlloc(ppd_attr->value);

  if ((ppd_attr = ppdFindAttr(ppd, "cupsMandatory", NULL)) != NULL)
    pc->mandatory = _cupsArrayNewStrings(ppd_attr->value, ' ');

 /*
  * Support files...
  */

  pc->support_files = cupsArrayNew3(NULL, NULL, NULL, 0,
				    (cups_acopy_func_t)_cupsStrAlloc,
				    (cups_afree_func_t)_cupsStrFree);

  for (ppd_attr = ppdFindAttr(ppd, "cupsICCProfile", NULL);
       ppd_attr;
       ppd_attr = ppdFindNextAttr(ppd, "cupsICCProfile", NULL))
    cupsArrayAdd(pc->support_files, ppd_attr->value);

  if ((ppd_attr = ppdFindAttr(ppd, "APPrinterIconPath", NULL)) != NULL)
    cupsArrayAdd(pc->support_files, ppd_attr->value);

 /*
  * 3D stuff...
  */

  if ((ppd_attr = ppdFindAttr(ppd, "cups3D", NULL)) != NULL)
    pc->cups_3d = _cupsStrAlloc(ppd_attr->value);

  if ((ppd_attr = ppdFindAttr(ppd, "cupsLayerOrder", NULL)) != NULL)
    pc->cups_layer_order = _cupsStrAlloc(ppd_attr->value);

  if ((ppd_attr = ppdFindAttr(ppd, "cupsAccuracy", NULL)) != NULL)
    sscanf(ppd_attr->value, "%d%d%d", pc->cups_accuracy + 0, pc->cups_accuracy + 1, pc->cups_accuracy + 2);

  if ((ppd_attr = ppdFindAttr(ppd, "cupsVolume", NULL)) != NULL)
    sscanf(ppd_attr->value, "%d%d%d", pc->cups_volume + 0, pc->cups_volume + 1, pc->cups_volume + 2);

  for (ppd_attr = ppdFindAttr(ppd, "cupsMaterial", NULL);
       ppd_attr;
       ppd_attr = ppdFindNextAttr(ppd, "cupsMaterial", NULL))
  {
   /*
    * *cupsMaterial key/name: "name=value ... name=value"
    */

    _pwg_material_t	*material = (_pwg_material_t *)calloc(1, sizeof(_pwg_material_t));

    material->key = _cupsStrAlloc(ppd_attr->name);
    material->name = _cupsStrAlloc(ppd_attr->text);
    material->num_props = cupsParseOptions(ppd_attr->value, 0, &material->props);

    if (!pc->materials)
      pc->materials = cupsArrayNew3(NULL, NULL, NULL, 0, NULL, (cups_afree_func_t)pwg_free_material);

    cupsArrayAdd(pc->materials, material);
  }

 /*
  * Return the cache data...
  */

  return (pc);

 /*
  * If we get here we need to destroy the PWG mapping data and return NULL...
  */

  create_error:

  _cupsSetError(IPP_STATUS_ERROR_INTERNAL, _("Out of memory."), 1);
  _ppdCacheDestroy(pc);

  return (NULL);
}