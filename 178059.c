CURLcode Curl_open(struct Curl_easy **curl)
{
  CURLcode result;
  struct Curl_easy *data;

  /* Very simple start-up: alloc the struct, init it with zeroes and return */
  data = calloc(1, sizeof(struct Curl_easy));
  if(!data) {
    /* this is a very serious error */
    DEBUGF(fprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    return CURLE_OUT_OF_MEMORY;
  }

  data->magic = CURLEASY_MAGIC_NUMBER;

  result = Curl_resolver_init(&data->state.resolver);
  if(result) {
    DEBUGF(fprintf(stderr, "Error: resolver_init failed\n"));
    free(data);
    return result;
  }

  /* We do some initial setup here, all those fields that can't be just 0 */

  data->state.buffer = malloc(READBUFFER_SIZE + 1);
  if(!data->state.buffer) {
    DEBUGF(fprintf(stderr, "Error: malloc of buffer failed\n"));
    result = CURLE_OUT_OF_MEMORY;
  }
  else {
    Curl_mime_initpart(&data->set.mimepost, data);

    data->state.headerbuff = malloc(HEADERSIZE);
    if(!data->state.headerbuff) {
      DEBUGF(fprintf(stderr, "Error: malloc of headerbuff failed\n"));
      result = CURLE_OUT_OF_MEMORY;
    }
    else {
      result = Curl_init_userdefined(&data->set);

      data->state.headersize = HEADERSIZE;
      Curl_convert_init(data);
      Curl_initinfo(data);

      /* most recent connection is not yet defined */
      data->state.lastconnect = NULL;

      data->progress.flags |= PGRS_HIDE;
      data->state.current_speed = -1; /* init to negative == impossible */
      data->set.fnmatch = ZERO_NULL;
      data->set.maxconnects = DEFAULT_CONNCACHE_SIZE; /* for easy handles */

      Curl_http2_init_state(&data->state);
    }
  }

  if(result) {
    Curl_resolver_cleanup(data->state.resolver);
    free(data->state.buffer);
    free(data->state.headerbuff);
    Curl_freeset(data);
    free(data);
    data = NULL;
  }
  else
    *curl = data;

  return result;
}