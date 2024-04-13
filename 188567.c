GList *ga_command_blacklist_init(GList *blacklist)
{
    const char *list_unsupported[] = {
        "guest-suspend-hybrid",
        "guest-set-vcpus",
        "guest-get-memory-blocks", "guest-set-memory-blocks",
        "guest-get-memory-block-size",
        "guest-fsfreeze-freeze-list",
        NULL};
    char **p = (char **)list_unsupported;

    while (*p) {
        blacklist = g_list_append(blacklist, g_strdup(*p++));
    }

    if (!vss_init(true)) {
        g_debug("vss_init failed, vss commands are going to be disabled");
        const char *list[] = {
            "guest-get-fsinfo", "guest-fsfreeze-status",
            "guest-fsfreeze-freeze", "guest-fsfreeze-thaw", NULL};
        p = (char **)list;

        while (*p) {
            blacklist = g_list_append(blacklist, g_strdup(*p++));
        }
    }

    return blacklist;
}