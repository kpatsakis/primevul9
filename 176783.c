bool Curl_meets_timecondition(struct Curl_easy *data, time_t timeofdoc)
{
  if((timeofdoc == 0) || (data->set.timevalue == 0))
    return TRUE;

  switch(data->set.timecondition) {
  case CURL_TIMECOND_IFMODSINCE:
  default:
    if(timeofdoc <= data->set.timevalue) {
      infof(data,
            "The requested document is not new enough\n");
      data->info.timecond = TRUE;
      return FALSE;
    }
    break;
  case CURL_TIMECOND_IFUNMODSINCE:
    if(timeofdoc >= data->set.timevalue) {
      infof(data,
            "The requested document is not old enough\n");
      data->info.timecond = TRUE;
      return FALSE;
    }
    break;
  }

  return TRUE;
}