_zip_u2d_time(time_t time, unsigned short *dtime, unsigned short *ddate)
{
    struct tm *tm;

    tm = localtime(&time);
    *ddate = ((tm->tm_year+1900-1980)<<9) + ((tm->tm_mon+1)<<5)
	+ tm->tm_mday;
    *dtime = ((tm->tm_hour)<<11) + ((tm->tm_min)<<5)
	+ ((tm->tm_sec)>>1);

    return;
}