static char *get_pci_driver(char const *syspath, int pathlen, Error **errp)
{
    char *path;
    char *dpath;
    char *driver = NULL;
    char buf[PATH_MAX];
    ssize_t len;

    path = g_strndup(syspath, pathlen);
    dpath = g_strdup_printf("%s/driver", path);
    len = readlink(dpath, buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = 0;
        driver = g_path_get_basename(buf);
    }
    g_free(dpath);
    g_free(path);
    return driver;
}