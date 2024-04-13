loop_check(const char **spec, const char **type, int *flags,
	   int *loop, const char **loopdev, const char **loopfile,
	   const char *node) {
  int looptype;
  uintmax_t offset = 0, sizelimit = 0;
  struct loopdev_cxt lc;
  char *pwd = NULL;
  int ret = EX_FAIL;

  /*
   * In the case of a loop mount, either type is of the form lo@/dev/loop5
   * or the option "-o loop=/dev/loop5" or just "-o loop" is given, or
   * mount just has to figure things out for itself from the fact that
   * spec is not a block device. We do not test for a block device
   * immediately: maybe later other types of mountable objects will occur.
   */

  *loopdev = opt_loopdev;

  looptype = (*type && strncmp("lo@", *type, 3) == 0);
  if (looptype) {
    if (*loopdev)
      error(_("mount: loop device specified twice"));
    *loopdev = *type + 3;
    *type = opt_vfstype;
  } else if (opt_vfstype) {
    if (*type)
      error(_("mount: type specified twice"));
    else
      *type = opt_vfstype;
  }

  *loop = ((*flags & MS_LOOP) || *loopdev || opt_offset || opt_sizelimit);
  *loopfile = *spec;

  /* Automatically create a loop device from a regular file if a filesystem
   * is not specified or the filesystem is known for libblkid (these
   * filesystems work with block devices only).
   *
   * Note that there is not a restriction (on kernel side) that prevents regular
   * file as a mount(2) source argument. A filesystem that is able to mount
   * regular files could be implemented.
   */
  if (!*loop && !(*flags & (MS_BIND | MS_MOVE | MS_PROPAGATION)) &&
      (!*type || strcmp(*type, "auto") == 0 || fsprobe_known_fstype(*type))) {

    struct stat st;
    if (stat(*loopfile, &st) == 0)
      *loop = S_ISREG(st.st_mode);
  }

  if (*loop) {
    *flags |= MS_LOOP;
    if (fake) {
      if (verbose)
	printf(_("mount: skipping the setup of a loop device\n"));
    } else {
      int loop_opts = 0;

      /* since 2.6.37 we don't have to store backing filename to mtab
       * because kernel provides the name in /sys
       */
      if (get_linux_version() >= KERNEL_VERSION(2, 6, 37) ||
	  mtab_is_writable() == 0) {

	if (verbose)
	  printf(_("mount: enabling autoclear loopdev flag\n"));
	loop_opts = LO_FLAGS_AUTOCLEAR;
      }

      if (*flags & MS_RDONLY)
        loop_opts |= LO_FLAGS_READ_ONLY;

      if (opt_offset && parse_offset(&opt_offset, &offset)) {
        error(_("mount: invalid offset '%s' specified"), opt_offset);
        return EX_FAIL;
      }
      if (opt_sizelimit && parse_offset(&opt_sizelimit, &sizelimit)) {
        error(_("mount: invalid sizelimit '%s' specified"), opt_sizelimit);
        return EX_FAIL;
      }

      if (is_mounted_same_loopfile(node, *loopfile, offset)) {
        error(_("mount: according to mtab %s is already mounted on %s as loop"), *loopfile, node);
        return EX_FAIL;
      }

      if (opt_encryption) {
        error("mount: %s", _("encryption not supported, use cryptsetup(8) instead"));
        return EX_FAIL;
      }

      loopcxt_init(&lc, 0);
      /*loopcxt_enable_debug(&lc, 1);*/

      if (*loopdev && **loopdev)
	loopcxt_set_device(&lc, *loopdev);	/* use loop=<devname> */

      do {
	int rc;

        if ((!*loopdev || !**loopdev) && loopcxt_find_unused(&lc) == 0)
	    *loopdev = loopcxt_strdup_device(&lc);

	if (!*loopdev) {
	  error(_("mount: failed to found free loop device"));
	  loopcxt_deinit(&lc);
	  goto err;	/* no more loop devices */
	}
	if (verbose)
	  printf(_("mount: going to use the loop device %s\n"), *loopdev);

	rc = loopcxt_set_backing_file(&lc, *loopfile);

	if (!rc && offset)
	  rc = loopcxt_set_offset(&lc, offset);
	if (!rc && sizelimit)
	  rc = loopcxt_set_sizelimit(&lc, sizelimit);
	if (!rc)
	  loopcxt_set_flags(&lc, loop_opts);

	if (rc) {
	   error(_("mount: %s: failed to set loopdev attributes"), *loopdev);
	   loopcxt_deinit(&lc);
	   goto err;
	}

	/* setup the device */
	rc = loopcxt_setup_device(&lc);
	if (!rc)
	  break;	/* success */

	if (rc != -EBUSY) {
	  error(_("mount: %s: failed setting up loop device: %m"), *loopfile);
	  if (!opt_loopdev) {
	    my_free(*loopdev);
	    *loopdev = NULL;
	  }
	  loopcxt_deinit(&lc);
	  goto err;
	}

	if (!opt_loopdev) {
	  if (verbose)
	    printf(_("mount: stolen loop=%s ...trying again\n"), *loopdev);
	    my_free(*loopdev);
	    *loopdev = NULL;
	    continue;
	}
	error(_("mount: stolen loop=%s"), *loopdev);
	loopcxt_deinit(&lc);
	goto err;

      } while (!*loopdev);

      if (verbose > 1)
	printf(_("mount: setup loop device successfully\n"));
      *spec = *loopdev;

      if (loopcxt_is_readonly(&lc))
        *flags |= MS_RDONLY;

      if (loopcxt_is_autoclear(&lc))
        /* Prevent recording loop dev in mtab for cleanup on umount */
        *loop = 0;

      /* We have to keep the device open until mount(2), otherwise it will
       * be auto-cleared by kernel (because LO_FLAGS_AUTOCLEAR) */
      loopcxt_set_fd(&lc, -1, 0);
      loopcxt_deinit(&lc);
    }
  }

  ret = 0;
err:
  if (pwd) {
    char *p = pwd;
    while (*p)
      *p++ = '\0';
    free(pwd);
  }
  return ret;
}