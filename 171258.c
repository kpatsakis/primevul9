gst_date_time_check_fields (gint * year, gint * month, gint * day,
    gint * hour, gint * minute, gdouble * seconds)
{
  if (*month == -1) {
    *month = *day = 1;
    *hour = *minute = *seconds = 0;
    return GST_DATE_TIME_FIELDS_Y;
  } else if (*day == -1) {
    *day = 1;
    *hour = *minute = *seconds = 0;
    return GST_DATE_TIME_FIELDS_YM;
  } else if (*hour == -1) {
    *hour = *minute = *seconds = 0;
    return GST_DATE_TIME_FIELDS_YMD;
  } else if (*seconds == -1) {
    *seconds = 0;
    return GST_DATE_TIME_FIELDS_YMD_HM;
  } else
    return GST_DATE_TIME_FIELDS_YMD_HMS;
}