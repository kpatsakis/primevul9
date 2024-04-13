dnsc_chroot_path(struct config_file *cfg, char *path)
{
    char *nm;
    nm = path;
    if(cfg->chrootdir && cfg->chrootdir[0] && strncmp(nm,
        cfg->chrootdir, strlen(cfg->chrootdir)) == 0)
        nm += strlen(cfg->chrootdir);
    return nm;
}