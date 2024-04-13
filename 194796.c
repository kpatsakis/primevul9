hook_timer_init (struct t_hook *hook)
{
    time_t time_now;
    struct tm *local_time, *gm_time;
    int local_hour, gm_hour, diff_hour;

    gettimeofday (&HOOK_TIMER(hook, last_exec), NULL);
    time_now = time (NULL);
    local_time = localtime(&time_now);
    local_hour = local_time->tm_hour;
    gm_time = gmtime(&time_now);
    gm_hour = gm_time->tm_hour;
    if ((local_time->tm_year > gm_time->tm_year)
        || (local_time->tm_mon > gm_time->tm_mon)
        || (local_time->tm_mday > gm_time->tm_mday))
    {
        diff_hour = (24 - gm_hour) + local_hour;
    }
    else if ((gm_time->tm_year > local_time->tm_year)
             || (gm_time->tm_mon > local_time->tm_mon)
             || (gm_time->tm_mday > local_time->tm_mday))
    {
        diff_hour = (-1) * ((24 - local_hour) + gm_hour);
    }
    else
        diff_hour = local_hour - gm_hour;

    if ((HOOK_TIMER(hook, interval) >= 1000)
        && (HOOK_TIMER(hook, align_second) > 0))
    {
        /*
         * here we should use 0, but with this value timer is sometimes called
         * before second has changed, so for displaying time, it may display
         * 2 times the same second, that's why we use 1000 micro seconds
         */
        HOOK_TIMER(hook, last_exec).tv_usec = 1000;
        HOOK_TIMER(hook, last_exec).tv_sec =
            HOOK_TIMER(hook, last_exec).tv_sec -
            ((HOOK_TIMER(hook, last_exec).tv_sec + (diff_hour * 3600)) %
             HOOK_TIMER(hook, align_second));
    }

    /* init next call with date of last call */
    HOOK_TIMER(hook, next_exec).tv_sec = HOOK_TIMER(hook, last_exec).tv_sec;
    HOOK_TIMER(hook, next_exec).tv_usec = HOOK_TIMER(hook, last_exec).tv_usec;

    /* add interval to next call date */
    util_timeval_add (&HOOK_TIMER(hook, next_exec), HOOK_TIMER(hook, interval));
}