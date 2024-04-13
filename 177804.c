open_temp_exec_file_mnt (const char *mounts)
{
  static const char *last_mounts;
  static FILE *last_mntent;

  if (mounts != last_mounts)
    {
      if (last_mntent)
	endmntent (last_mntent);

      last_mounts = mounts;

      if (mounts)
	last_mntent = setmntent (mounts, "r");
      else
	last_mntent = NULL;
    }

  if (!last_mntent)
    return -1;

  for (;;)
    {
      int fd;
      struct mntent mnt;
      char buf[MAXPATHLEN * 3];

      if (getmntent_r (last_mntent, &mnt, buf, sizeof (buf)) == NULL)
	return -1;

      if (hasmntopt (&mnt, "ro")
	  || hasmntopt (&mnt, "noexec")
	  || access (mnt.mnt_dir, W_OK))
	continue;

      fd = open_temp_exec_file_dir (mnt.mnt_dir);

      if (fd != -1)
	return fd;
    }
}