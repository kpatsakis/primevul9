static void free_family_stat(QuicUsrContext *usr, FamilyStat *family_stat)
{
    usr->free(usr, family_stat->buckets_ptrs);
    usr->free(usr, family_stat->counters);
    usr->free(usr, family_stat->buckets_buf);
}