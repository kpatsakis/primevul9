static double ga_get_login_time(struct utmpx *user_info)
{
    double seconds = (double)user_info->ut_tv.tv_sec;
    double useconds = (double)user_info->ut_tv.tv_usec;
    useconds /= QGA_MICRO_SECOND_TO_SECOND;
    return seconds + useconds;
}