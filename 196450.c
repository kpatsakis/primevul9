CURLcode Curl_add_timecondition(struct SessionHandle *data,
                                Curl_send_buffer *req_buffer)
{
  const struct tm *tm;
  char *buf = data->state.buffer;
  struct tm keeptime;
  CURLcode result = Curl_gmtime(data->set.timevalue, &keeptime);
  if(result) {
    failf(data, "Invalid TIMEVALUE");
    return result;
  }
  tm = &keeptime;

  /* The If-Modified-Since header family should have their times set in
   * GMT as RFC2616 defines: "All HTTP date/time stamps MUST be
   * represented in Greenwich Mean Time (GMT), without exception. For the
   * purposes of HTTP, GMT is exactly equal to UTC (Coordinated Universal
   * Time)." (see page 20 of RFC2616).
   */

  /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
  snprintf(buf, BUFSIZE-1,
           "%s, %02d %s %4d %02d:%02d:%02d GMT",
           Curl_wkday[tm->tm_wday?tm->tm_wday-1:6],
           tm->tm_mday,
           Curl_month[tm->tm_mon],
           tm->tm_year + 1900,
           tm->tm_hour,
           tm->tm_min,
           tm->tm_sec);

  switch(data->set.timecondition) {
  case CURL_TIMECOND_IFMODSINCE:
  default:
    result = Curl_add_bufferf(req_buffer,
                              "If-Modified-Since: %s\r\n", buf);
    break;
  case CURL_TIMECOND_IFUNMODSINCE:
    result = Curl_add_bufferf(req_buffer,
                              "If-Unmodified-Since: %s\r\n", buf);
    break;
  case CURL_TIMECOND_LASTMOD:
    result = Curl_add_bufferf(req_buffer,
                              "Last-Modified: %s\r\n", buf);
    break;
  }

  return result;
}