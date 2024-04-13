CURLcode operate(struct GlobalConfig *config, int argc, argv_item_t argv[])
{
  CURLcode result = CURLE_OK;

  /* Setup proper locale from environment */
#ifdef HAVE_SETLOCALE
  setlocale(LC_ALL, "");
#endif

  /* Parse .curlrc if necessary */
  if((argc == 1) || (!curlx_strequal(argv[1], "-q"))) {
    parseconfig(NULL, config); /* ignore possible failure */

    /* If we had no arguments then make sure a url was specified in .curlrc */
    if((argc < 2) && (!config->first->url_list)) {
      helpf(config->errors, NULL);
      result = CURLE_FAILED_INIT;
    }
  }

  if(!result) {
    /* Parse the command line arguments */
    ParameterError res = parse_args(config, argc, argv);
    if(res) {
      result = CURLE_OK;

      /* Check if we were asked for the help */
      if(res == PARAM_HELP_REQUESTED)
        tool_help();
      /* Check if we were asked for the manual */
      else if(res == PARAM_MANUAL_REQUESTED)
        hugehelp();
      /* Check if we were asked for the version information */
      else if(res == PARAM_VERSION_INFO_REQUESTED)
        tool_version_info();
      /* Check if we were asked to list the SSL engines */
      else if(res == PARAM_ENGINES_REQUESTED)
        tool_list_engines(config->easy);
      else if(res == PARAM_LIBCURL_UNSUPPORTED_PROTOCOL)
        result = CURLE_UNSUPPORTED_PROTOCOL;
      else
        result = CURLE_FAILED_INIT;
    }
    else {
#ifndef CURL_DISABLE_LIBCURL_OPTION
      /* Initialise the libcurl source output */
      result = easysrc_init();
#endif

      /* Perform the main operations */
      if(!result) {
        size_t count = 0;
        struct OperationConfig *operation = config->first;

        /* Get the required aguments for each operation */
        while(!result && operation) {
          result = get_args(operation, count++);

          operation = operation->next;
        }

        /* Set the current operation pointer */
        config->current = config->first;

        /* Perform each operation */
        while(!result && config->current) {
          result = operate_do(config, config->current);

          config->current = config->current->next;
        }

#ifndef CURL_DISABLE_LIBCURL_OPTION
        /* Cleanup the libcurl source output */
        easysrc_cleanup();

        /* Dump the libcurl code if previously enabled */
        dumpeasysrc(config);
#endif
      }
      else
        helpf(config->errors, "out of memory\n");
    }
  }

  return result;
}