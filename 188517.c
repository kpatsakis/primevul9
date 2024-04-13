GList *ga_command_blacklist_init(GList *blacklist)
{
#if !defined(__linux__)
    {
        const char *list[] = {
            "guest-suspend-disk", "guest-suspend-ram",
            "guest-suspend-hybrid", "guest-network-get-interfaces",
            "guest-get-vcpus", "guest-set-vcpus",
            "guest-get-memory-blocks", "guest-set-memory-blocks",
            "guest-get-memory-block-size", NULL};
        char **p = (char **)list;

        while (*p) {
            blacklist = g_list_append(blacklist, g_strdup(*p++));
        }
    }
#endif

#if !defined(CONFIG_FSFREEZE)
    {
        const char *list[] = {
            "guest-get-fsinfo", "guest-fsfreeze-status",
            "guest-fsfreeze-freeze", "guest-fsfreeze-freeze-list",
            "guest-fsfreeze-thaw", "guest-get-fsinfo", NULL};
        char **p = (char **)list;

        while (*p) {
            blacklist = g_list_append(blacklist, g_strdup(*p++));
        }
    }
#endif

#if !defined(CONFIG_FSTRIM)
    blacklist = g_list_append(blacklist, g_strdup("guest-fstrim"));
#endif

    return blacklist;
}