static int stat_to_v9stat_dotl(V9fsPDU *pdu, const struct stat *stbuf,
                                V9fsStatDotl *v9lstat)
{
    memset(v9lstat, 0, sizeof(*v9lstat));

    v9lstat->st_mode = stbuf->st_mode;
    v9lstat->st_nlink = stbuf->st_nlink;
    v9lstat->st_uid = stbuf->st_uid;
    v9lstat->st_gid = stbuf->st_gid;
    v9lstat->st_rdev = stbuf->st_rdev;
    v9lstat->st_size = stbuf->st_size;
    v9lstat->st_blksize = stbuf->st_blksize;
    v9lstat->st_blocks = stbuf->st_blocks;
    v9lstat->st_atime_sec = stbuf->st_atime;
    v9lstat->st_atime_nsec = stbuf->st_atim.tv_nsec;
    v9lstat->st_mtime_sec = stbuf->st_mtime;
    v9lstat->st_mtime_nsec = stbuf->st_mtim.tv_nsec;
    v9lstat->st_ctime_sec = stbuf->st_ctime;
    v9lstat->st_ctime_nsec = stbuf->st_ctim.tv_nsec;
    /* Currently we only support BASIC fields in stat */
    v9lstat->st_result_mask = P9_STATS_BASIC;

    return stat_to_qid(pdu, stbuf, &v9lstat->qid);
}