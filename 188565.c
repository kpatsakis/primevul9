static int build_hosts(char const *syspath, char const *host, bool ata,
                       unsigned int *hosts, int hosts_max, Error **errp)
{
    char *path;
    DIR *dir;
    struct dirent *entry;
    int i = 0;

    path = g_strndup(syspath, host - syspath);
    dir = opendir(path);
    if (!dir) {
        error_setg_errno(errp, errno, "opendir(\"%s\")", path);
        g_free(path);
        return -1;
    }

    while (i < hosts_max) {
        entry = readdir(dir);
        if (!entry) {
            break;
        }
        if (ata && sscanf(entry->d_name, "ata%d", hosts + i) == 1) {
            ++i;
        } else if (!ata && sscanf(entry->d_name, "host%d", hosts + i) == 1) {
            ++i;
        }
    }

    qsort(hosts, i, sizeof(hosts[0]), compare_uint);

    g_free(path);
    closedir(dir);
    return i;
}