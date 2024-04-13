_cupsConvertOptions(ipp_t           *request,	/* I - IPP request */
                    ppd_file_t      *ppd,	/* I - PPD file */
		    _ppd_cache_t    *pc,	/* I - PPD cache info */
		    ipp_attribute_t *media_col_sup,
						/* I - media-col-supported values */
		    ipp_attribute_t *doc_handling_sup,
						/* I - multiple-document-handling-supported values */
		    ipp_attribute_t *print_color_mode_sup,
						/* I - Printer supports print-color-mode */
		    const char    *user,	/* I - User info */
		    const char    *format,	/* I - document-format value */
		    int           copies,	/* I - Number of copies */
		    int           num_options,	/* I - Number of options */
		    cups_option_t *options)	/* I - Options */
{
  int		i;			/* Looping var */
  const char	*keyword,		/* PWG keyword */
		*password;		/* Password string */
  pwg_size_t	*size;			/* PWG media size */
  ipp_t		*media_col,		/* media-col value */
		*media_size;		/* media-size value */
  const char	*media_source,		/* media-source value */
		*media_type,		/* media-type value */
		*collate_str,		/* multiple-document-handling value */
		*color_attr_name,	/* Supported color attribute */
		*mandatory;		/* Mandatory attributes */
  int		num_finishings = 0,	/* Number of finishing values */
		finishings[10];		/* Finishing enum values */
  ppd_choice_t	*choice;		/* Marked choice */


 /*
  * Send standard IPP attributes...
  */

  if (pc->password && (password = cupsGetOption("job-password", num_options, options)) != NULL && ippGetOperation(request) != IPP_OP_VALIDATE_JOB)
  {
    ipp_attribute_t	*attr = NULL;	/* job-password attribute */

    if ((keyword = cupsGetOption("job-password-encryption", num_options, options)) == NULL)
      keyword = "none";

    if (!strcmp(keyword, "none"))
    {
     /*
      * Add plain-text job-password...
      */

      attr = ippAddOctetString(request, IPP_TAG_OPERATION, "job-password", password, (int)strlen(password));
    }
    else
    {
     /*
      * Add hashed job-password...
      */

      unsigned char	hash[64];	/* Hash of password */
      ssize_t		hashlen;	/* Length of hash */

      if ((hashlen = cupsHashData(keyword, password, strlen(password), hash, sizeof(hash))) > 0)
        attr = ippAddOctetString(request, IPP_TAG_OPERATION, "job-password", hash, (int)hashlen);
    }

    if (attr)
      ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD, "job-password-encryption", NULL, keyword);
  }

  if (pc->account_id)
  {
    if ((keyword = cupsGetOption("job-account-id", num_options, options)) == NULL)
      keyword = cupsGetOption("job-billing", num_options, options);

    if (keyword)
      ippAddString(request, IPP_TAG_JOB, IPP_TAG_NAME, "job-account-id", NULL, keyword);
  }

  if (pc->accounting_user_id)
  {
    if ((keyword = cupsGetOption("job-accounting-user-id", num_options, options)) == NULL)
      keyword = user;

    if (keyword)
      ippAddString(request, IPP_TAG_JOB, IPP_TAG_NAME, "job-accounting-user-id", NULL, keyword);
  }

  for (mandatory = (const char *)cupsArrayFirst(pc->mandatory); mandatory; mandatory = (const char *)cupsArrayNext(pc->mandatory))
  {
    if (strcmp(mandatory, "copies") &&
	strcmp(mandatory, "destination-uris") &&
	strcmp(mandatory, "finishings") &&
	strcmp(mandatory, "job-account-id") &&
	strcmp(mandatory, "job-accounting-user-id") &&
	strcmp(mandatory, "job-password") &&
	strcmp(mandatory, "job-password-encryption") &&
	strcmp(mandatory, "media") &&
	strncmp(mandatory, "media-col", 9) &&
	strcmp(mandatory, "multiple-document-handling") &&
	strcmp(mandatory, "output-bin") &&
	strcmp(mandatory, "print-color-mode") &&
	strcmp(mandatory, "print-quality") &&
	strcmp(mandatory, "sides") &&
	(keyword = cupsGetOption(mandatory, num_options, options)) != NULL)
    {
      _ipp_option_t *opt = _ippFindOption(mandatory);
				    /* Option type */
      ipp_tag_t	value_tag = opt ? opt->value_tag : IPP_TAG_NAME;
				    /* Value type */

      switch (value_tag)
      {
	case IPP_TAG_INTEGER :
	case IPP_TAG_ENUM :
	    ippAddInteger(request, IPP_TAG_JOB, value_tag, mandatory, atoi(keyword));
	    break;
	case IPP_TAG_BOOLEAN :
	    ippAddBoolean(request, IPP_TAG_JOB, mandatory, !_cups_strcasecmp(keyword, "true"));
	    break;
	case IPP_TAG_RANGE :
	    {
	      int lower, upper;	/* Range */

	      if (sscanf(keyword, "%d-%d", &lower, &upper) != 2)
		lower = upper = atoi(keyword);

	      ippAddRange(request, IPP_TAG_JOB, mandatory, lower, upper);
	    }
	    break;
	case IPP_TAG_STRING :
	    ippAddOctetString(request, IPP_TAG_JOB, mandatory, keyword, (int)strlen(keyword));
	    break;
	default :
	    if (!strcmp(mandatory, "print-color-mode") && !strcmp(keyword, "monochrome"))
	    {
	      if (ippContainsString(print_color_mode_sup, "auto-monochrome"))
		keyword = "auto-monochrome";
	      else if (ippContainsString(print_color_mode_sup, "process-monochrome") && !ippContainsString(print_color_mode_sup, "monochrome"))
		keyword = "process-monochrome";
	    }

	    ippAddString(request, IPP_TAG_JOB, value_tag, mandatory, NULL, keyword);
	    break;
      }
    }
  }

  if ((keyword = cupsGetOption("PageSize", num_options, options)) == NULL)
    keyword = cupsGetOption("media", num_options, options);

  if ((size = _ppdCacheGetSize(pc, keyword)) != NULL)
  {
   /*
    * Add a media-col value...
    */

    media_size = ippNew();
    ippAddInteger(media_size, IPP_TAG_ZERO, IPP_TAG_INTEGER,
		  "x-dimension", size->width);
    ippAddInteger(media_size, IPP_TAG_ZERO, IPP_TAG_INTEGER,
		  "y-dimension", size->length);

    media_col = ippNew();
    ippAddCollection(media_col, IPP_TAG_ZERO, "media-size", media_size);

    media_source = _ppdCacheGetSource(pc, cupsGetOption("InputSlot",
							num_options,
							options));
    media_type   = _ppdCacheGetType(pc, cupsGetOption("MediaType",
						      num_options,
						      options));

    for (i = 0; i < media_col_sup->num_values; i ++)
    {
      if (!strcmp(media_col_sup->values[i].string.text, "media-left-margin"))
	ippAddInteger(media_col, IPP_TAG_ZERO, IPP_TAG_INTEGER, "media-left-margin", size->left);
      else if (!strcmp(media_col_sup->values[i].string.text, "media-bottom-margin"))
	ippAddInteger(media_col, IPP_TAG_ZERO, IPP_TAG_INTEGER, "media-bottom-margin", size->bottom);
      else if (!strcmp(media_col_sup->values[i].string.text, "media-right-margin"))
	ippAddInteger(media_col, IPP_TAG_ZERO, IPP_TAG_INTEGER, "media-right-margin", size->right);
      else if (!strcmp(media_col_sup->values[i].string.text, "media-top-margin"))
	ippAddInteger(media_col, IPP_TAG_ZERO, IPP_TAG_INTEGER, "media-top-margin", size->top);
      else if (!strcmp(media_col_sup->values[i].string.text, "media-source") && media_source)
	ippAddString(media_col, IPP_TAG_ZERO, IPP_TAG_KEYWORD, "media-source", NULL, media_source);
      else if (!strcmp(media_col_sup->values[i].string.text, "media-type") && media_type)
	ippAddString(media_col, IPP_TAG_ZERO, IPP_TAG_KEYWORD, "media-type", NULL, media_type);
    }

    ippAddCollection(request, IPP_TAG_JOB, "media-col", media_col);
  }

  if ((keyword = cupsGetOption("output-bin", num_options, options)) == NULL)
  {
    if ((choice = ppdFindMarkedChoice(ppd, "OutputBin")) != NULL)
      keyword = _ppdCacheGetBin(pc, choice->choice);
  }

  if (keyword)
    ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "output-bin", NULL, keyword);

  color_attr_name = print_color_mode_sup ? "print-color-mode" : "output-mode";

  if ((keyword = cupsGetOption("print-color-mode", num_options, options)) == NULL)
  {
    if ((choice = ppdFindMarkedChoice(ppd, "ColorModel")) != NULL)
    {
      if (!_cups_strcasecmp(choice->choice, "Gray"))
	keyword = "monochrome";
      else
	keyword = "color";
    }
  }

  if (keyword && !strcmp(keyword, "monochrome"))
  {
    if (ippContainsString(print_color_mode_sup, "auto-monochrome"))
      keyword = "auto-monochrome";
    else if (ippContainsString(print_color_mode_sup, "process-monochrome") && !ippContainsString(print_color_mode_sup, "monochrome"))
      keyword = "process-monochrome";
  }

  if (keyword)
    ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, color_attr_name, NULL, keyword);

  if ((keyword = cupsGetOption("print-quality", num_options, options)) != NULL)
    ippAddInteger(request, IPP_TAG_JOB, IPP_TAG_ENUM, "print-quality", atoi(keyword));
  else if ((choice = ppdFindMarkedChoice(ppd, "cupsPrintQuality")) != NULL)
  {
    if (!_cups_strcasecmp(choice->choice, "draft"))
      ippAddInteger(request, IPP_TAG_JOB, IPP_TAG_ENUM, "print-quality", IPP_QUALITY_DRAFT);
    else if (!_cups_strcasecmp(choice->choice, "normal"))
      ippAddInteger(request, IPP_TAG_JOB, IPP_TAG_ENUM, "print-quality", IPP_QUALITY_NORMAL);
    else if (!_cups_strcasecmp(choice->choice, "high"))
      ippAddInteger(request, IPP_TAG_JOB, IPP_TAG_ENUM, "print-quality", IPP_QUALITY_HIGH);
  }

  if ((keyword = cupsGetOption("sides", num_options, options)) != NULL)
    ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "sides", NULL, keyword);
  else if (pc->sides_option && (choice = ppdFindMarkedChoice(ppd, pc->sides_option)) != NULL)
  {
    if (!_cups_strcasecmp(choice->choice, pc->sides_1sided))
      ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "sides", NULL, "one-sided");
    else if (!_cups_strcasecmp(choice->choice, pc->sides_2sided_long))
      ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "sides", NULL, "two-sided-long-edge");
    if (!_cups_strcasecmp(choice->choice, pc->sides_2sided_short))
      ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "sides", NULL, "two-sided-short-edge");
  }

 /*
  * Copies...
  */

  if ((keyword = cupsGetOption("multiple-document-handling", num_options, options)) != NULL)
  {
    if (strstr(keyword, "uncollated"))
      keyword = "false";
    else
      keyword = "true";
  }
  else if ((keyword = cupsGetOption("collate", num_options, options)) == NULL)
    keyword = "true";

  if (format)
  {
    if (!_cups_strcasecmp(format, "image/gif") ||
	!_cups_strcasecmp(format, "image/jp2") ||
	!_cups_strcasecmp(format, "image/jpeg") ||
	!_cups_strcasecmp(format, "image/png") ||
	!_cups_strcasecmp(format, "image/tiff") ||
	!_cups_strncasecmp(format, "image/x-", 8))
    {
     /*
      * Collation makes no sense for single page image formats...
      */

      keyword = "false";
    }
    else if (!_cups_strncasecmp(format, "image/", 6) ||
	     !_cups_strcasecmp(format, "application/vnd.cups-raster"))
    {
     /*
      * Multi-page image formats will have copies applied by the upstream
      * filters...
      */

      copies = 1;
    }
  }

  if (doc_handling_sup)
  {
    if (!_cups_strcasecmp(keyword, "true"))
      collate_str = "separate-documents-collated-copies";
    else
      collate_str = "separate-documents-uncollated-copies";

    for (i = 0; i < doc_handling_sup->num_values; i ++)
    {
      if (!strcmp(doc_handling_sup->values[i].string.text, collate_str))
      {
	ippAddString(request, IPP_TAG_JOB, IPP_TAG_KEYWORD, "multiple-document-handling", NULL, collate_str);
	break;
      }
    }

    if (i >= doc_handling_sup->num_values)
      copies = 1;
  }

 /*
  * Map finishing options...
  */

  num_finishings = _ppdCacheGetFinishingValues(pc, num_options, options, (int)(sizeof(finishings) / sizeof(finishings[0])), finishings);
  if (num_finishings > 0)
  {
    ippAddIntegers(request, IPP_TAG_JOB, IPP_TAG_ENUM, "finishings", num_finishings, finishings);

    if (copies > 1 && (keyword = cupsGetOption("job-impressions", num_options, options)) != NULL)
    {
     /*
      * Send job-pages-per-set attribute to apply finishings correctly...
      */

      ippAddInteger(request, IPP_TAG_JOB, IPP_TAG_INTEGER, "job-pages-per-set", atoi(keyword) / copies);
    }
  }

  return (copies);
}