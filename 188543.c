static void free_fs_mount_list(FsMountList *mounts)
{
     FsMount *mount, *temp;

     if (!mounts) {
         return;
     }

     QTAILQ_FOREACH_SAFE(mount, mounts, next, temp) {
         QTAILQ_REMOVE(mounts, mount, next);
         g_free(mount->dirname);
         g_free(mount->devtype);
         g_free(mount);
     }
}