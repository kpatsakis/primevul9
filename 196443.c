CURLcode Curl_add_bufferf(Curl_send_buffer *in, const char *fmt, ...)
{
  char *s;
  va_list ap;
  va_start(ap, fmt);
  s = vaprintf(fmt, ap); /* this allocs a new string to append */
  va_end(ap);

  if(s) {
    CURLcode result = Curl_add_buffer(in, s, strlen(s));
    free(s);
    return result;
  }
  /* If we failed, we cleanup the whole buffer and return error */
  free(in->buffer);
  free(in);
  return CURLE_OUT_OF_MEMORY;
}