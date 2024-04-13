_zip_d2u_time(int dtime, int ddate)
{
    struct tm tm;

    memset(&tm, 0, sizeof(tm));
    
    /* let mktime decide if DST is in effect */
    tm.tm_isdst = -1;
    
    tm.tm_year = ((ddate>>9)&127) + 1980 - 1900;
    tm.tm_mon = ((ddate>>5)&15) - 1;
    tm.tm_mday = ddate&31;

    tm.tm_hour = (dtime>>11)&31;
    tm.tm_min = (dtime>>5)&63;
    tm.tm_sec = (dtime<<1)&62;

    return mktime(&tm);
}