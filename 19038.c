dnscrypt_hrtime(void)
{
    struct timeval tv;
    uint64_t ts = (uint64_t)0U;
    int ret;

    ret = gettimeofday(&tv, NULL);
    if (ret == 0) {
        ts = (uint64_t)tv.tv_sec * 1000000U + (uint64_t)tv.tv_usec;
    } else {
        log_err("gettimeofday: %s", strerror(errno));
    }
    return ts;
}