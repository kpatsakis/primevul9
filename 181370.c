static void v9fs_stat_init(V9fsStat *stat)
{
    v9fs_string_init(&stat->name);
    v9fs_string_init(&stat->uid);
    v9fs_string_init(&stat->gid);
    v9fs_string_init(&stat->muid);
    v9fs_string_init(&stat->extension);
}