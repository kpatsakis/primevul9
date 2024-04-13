void plist_bin_init(void)
{
    /* init binary plist stuff */
#ifdef DEBUG
    char *env_debug = getenv("PLIST_BIN_DEBUG");
    if (env_debug && !strcmp(env_debug, "1")) {
        plist_bin_debug = 1;
    }
#endif
}