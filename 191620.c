_zip_u2d_time(time_t intime, zip_uint16_t *dtime, zip_uint16_t *ddate)
{
    struct tm *tm;

    tm = localtime(&intime);
    if (tm->tm_year < 80) {
	tm->tm_year = 80;
    }

    *ddate = (zip_uint16_t)(((tm->tm_year+1900-1980)<<9) + ((tm->tm_mon+1)<<5) + tm->tm_mday);
    *dtime = (zip_uint16_t)(((tm->tm_hour)<<11) + ((tm->tm_min)<<5) + ((tm->tm_sec)>>1));

    return;
}