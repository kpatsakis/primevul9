static int entropy_func_mutex(void *data, unsigned char *output, size_t len)
{
    int ret;
    /* lock 1 = entropy_func_mutex() */
    Curl_polarsslthreadlock_lock_function(1);
    ret = entropy_func(data, output, len);
    Curl_polarsslthreadlock_unlock_function(1);

    return ret;
}