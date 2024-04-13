lyp_check_date(struct ly_ctx *ctx, const char *date)
{
    int i;
    struct tm tm, tm_;
    char *r;

    assert(date);

    /* check format */
    for (i = 0; i < LY_REV_SIZE - 1; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') {
                goto error;
            }
        } else if (!isdigit(date[i])) {
            goto error;
        }
    }

    /* check content, e.g. 2018-02-31 */
    memset(&tm, 0, sizeof tm);
    r = strptime(date, "%Y-%m-%d", &tm);
    if (!r || r != &date[LY_REV_SIZE - 1]) {
        goto error;
    }
    /* set some arbitrary non-0 value in case DST changes, it could move the day otherwise */
    tm.tm_hour = 12;

    memcpy(&tm_, &tm, sizeof tm);
    mktime(&tm_); /* mktime modifies tm_ if it refers invalid date */
    if (tm.tm_mday != tm_.tm_mday) { /* e.g 2018-02-29 -> 2018-03-01 */
        /* checking days is enough, since other errors
         * have been checked by strptime() */
        goto error;
    }

    return EXIT_SUCCESS;

error:
    LOGVAL(ctx, LYE_INDATE, LY_VLOG_NONE, NULL, date);
    return EXIT_FAILURE;
}