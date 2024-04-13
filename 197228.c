try_mount_one (const char *spec0, const char *node0, const char *types0,
	       const char *opts0, int freq, int pass, int ro) {
  int res = 0, status = 0, special = 0;
  int mnt5_res = 0;		/* only for gcc */
  int mnt_err;
  int flags;
  char *extra_opts;		/* written in mtab */
  char *mount_opts;		/* actually used on system call */
  const char *opts, *spec, *node, *types;
  char *user = 0;
  int loop = 0;
  const char *loopdev = 0, *loopfile = 0;
  struct stat statbuf;

  /* copies for freeing on exit */
  const char *opts1, *spec1, *node1, *types1;

  if (verbose > 2) {
	  printf("mount: spec:  \"%s\"\n", spec0);
	  printf("mount: node:  \"%s\"\n", node0);
	  printf("mount: types: \"%s\"\n", types0);
	  printf("mount: opts:  \"%s\"\n", opts0);
  }

  spec = spec1 = xstrdup(spec0);
  node = node1 = xstrdup(node0);
  types = types1 = xstrdup(types0);
  opts = opts1 = xstrdup(opts0);

  parse_opts (opts, &flags, &extra_opts);
  mount_opts = xstrdup(extra_opts);

  /* quietly succeed for fstab entries that don't get mounted automatically */
  if (mount_all && (flags & MS_NOAUTO))
      goto out;

  restricted_check(spec, node, &flags, &user);

  /* The "mount -f" checks for for existing record in /etc/mtab (with
   * regular non-fake mount this is usually done by kernel)
   */
  if (!(flags & MS_REMOUNT) && fake && mounted (spec, node, NULL))
      die(EX_USAGE, _("mount: according to mtab, "
                      "%s is already mounted on %s\n"),
		      spec, node);

  if (opt_speed)
      cdrom_setspeed(spec);

  if (!(flags & MS_REMOUNT)) {
      /*
       * Don't set up a (new) loop device if we only remount - this left
       * stale assignments of files to loop devices. Nasty when used for
       * encryption.
       */
      res = loop_check(&spec, &types, &flags, &loop, &loopdev, &loopfile, node);
      if (res)
	  goto out;
  }

  if (loop)
      opt_loopdev = loopdev;

  if (flags & (MS_BIND | MS_MOVE | MS_PROPAGATION))
      types = "none";

#ifdef HAVE_LIBSELINUX
  if (flags & MS_REMOUNT) {
      /*
       * Linux kernel does not accept any selinux context option on remount
       */
      if (mount_opts) {
          char *tmp = mount_opts;
          mount_opts = remove_context_options(mount_opts);
          my_free(tmp);
      }

  } else if (types && strcmp(types, "tmpfs") == 0 && is_selinux_enabled() > 0 &&
	   !has_context_option(mount_opts)) {
      /*
       * Add rootcontext= mount option for tmpfs
       * https://bugzilla.redhat.com/show_bug.cgi?id=476964
       */
      security_context_t sc = NULL;

      if (getfilecon(node, &sc) > 0 && strcmp("unlabeled", sc))
	      append_context("rootcontext=", (char *) sc, &mount_opts);
      freecon(sc);
  }
#endif

  /*
   * Call mount.TYPE for types that require a separate mount program.
   * For the moment these types are ncpfs and smbfs. Maybe also vxfs.
   * All such special things must occur isolated in the types string.
   */
  if (check_special_mountprog(spec, node, types, flags, mount_opts, &status)) {
      res = status;
      goto out;
  }

  block_signals (SIG_BLOCK);

  if (!fake) {
    mnt5_res = guess_fstype_and_mount (spec, node, &types, flags & ~MS_NOSYS,
				       mount_opts, &special, &status);

    if (special) {
      block_signals (SIG_UNBLOCK);
      res = status;
      goto out;
    }
  }

  /* Kernel allows to use MS_RDONLY for bind mounts, but the read-only request
   * could be silently ignored. Check it to avoid 'ro' in mtab and 'rw' in
   * /proc/mounts.
   */
  if (!fake && mnt5_res == 0 &&
      (flags & MS_BIND) && (flags & MS_RDONLY) && !is_readonly(node)) {

      printf(_("mount: warning: %s seems to be mounted read-write.\n"), node);
      flags &= ~MS_RDONLY;
  }

  /* Kernel can silently add MS_RDONLY flag when mounting file system that
   * does not have write support. Check this to avoid 'ro' in /proc/mounts
   * and 'rw' in mtab.
   */
  if (!fake && mnt5_res == 0 &&
      !(flags & (MS_RDONLY | MS_PROPAGATION | MS_MOVE)) &&
      is_readonly(node)) {

      printf(_("mount: warning: %s seems to be mounted read-only.\n"), node);
      flags |= MS_RDONLY;
  }

  if (fake || mnt5_res == 0) {
      char *mo = fix_opts_string (flags & ~MS_NOMTAB, extra_opts, user, 0);
      const char *tp = types ? types : "unknown";

      /* Mount succeeded, report this (if verbose) and write mtab entry.  */
      if (!(mounttype & MS_PROPAGATION))
	      update_mtab_entry(loop ? loopfile : spec,
			node,
			tp,
			mo,
			flags,
			freq,
			pass);

      block_signals (SIG_UNBLOCK);
      free(mo);

      res = 0;
      goto out;
  }

  mnt_err = errno;

  if (loop)
	loopdev_delete(spec);

  block_signals (SIG_UNBLOCK);

  /* Mount failed, complain, but don't die.  */

  if (types == 0) {
    if (restricted)
      error (_("mount: I could not determine the filesystem type, "
	       "and none was specified"));
    else
      error (_("mount: you must specify the filesystem type"));
  } else if (mnt5_res != -1) {
      /* should not happen */
      error (_("mount: mount failed"));
  } else {
   switch (mnt_err) {
    case EPERM:
      if (geteuid() == 0) {
	   if (stat (node, &statbuf) || !S_ISDIR(statbuf.st_mode))
		error (_("mount: mount point %s is not a directory"), node);
	   else
		error (_("mount: permission denied"));
      } else
	error (_("mount: must be superuser to use mount"));
      break;
    case EBUSY:
      if (flags & MS_REMOUNT) {
	error (_("mount: %s is busy"), node);
      } else if (!strcmp(types, "proc") && !strcmp(node, "/proc")) {
	/* heuristic: if /proc/version exists, then probably proc is mounted */
	if (stat ("/proc/version", &statbuf))   /* proc mounted? */
	   error (_("mount: %s is busy"), node);   /* no */
	else if (!mount_all || verbose)            /* yes, don't mention it */
	   error (_("mount: proc already mounted"));
      } else {
	error (_("mount: %s already mounted or %s busy"), spec, node);
	already (spec, node);
      }
      break;
    case ENOENT:
      if (lstat (node, &statbuf))
	   error (_("mount: mount point %s does not exist"), node);
      else if (stat (node, &statbuf))
	   error (_("mount: mount point %s is a symbolic link to nowhere"),
		  node);
      else if (stat (spec, &statbuf)) {
	   if (opt_nofail)
		goto out;
	   error (_("mount: special device %s does not exist"), spec);
      } else {
	   errno = mnt_err;
	   perror("mount");
      }
      break;
    case ENOTDIR:
      if (stat (node, &statbuf) || ! S_ISDIR(statbuf.st_mode))
	   error (_("mount: mount point %s is not a directory"), node);
      else if (stat (spec, &statbuf) && errno == ENOTDIR) {
	   if (opt_nofail)
              goto out;
	   error (_("mount: special device %s does not exist\n"
		    "       (a path prefix is not a directory)\n"), spec);
      } else {
	   errno = mnt_err;
	   perror("mount");
      }
      break;
    case EINVAL:
    { int fd;
      unsigned long long size = 0;

      if (flags & MS_REMOUNT) {
	error (_("mount: %s not mounted or bad option"), node);
      } else {
	error (_("mount: wrong fs type, bad option, bad superblock on %s,\n"
	       "       missing codepage or helper program, or other error"),
	       spec);

	if (stat(spec, &statbuf) < 0) {
	  if (errno == ENOENT)         /* network FS? */
	    error(_(
	       "       (for several filesystems (e.g. nfs, cifs) you might\n"
	       "       need a /sbin/mount.<type> helper program)"));

	} else if (S_ISBLK(statbuf.st_mode)
	                 && (fd = open(spec, O_RDONLY | O_NONBLOCK)) >= 0) {

	  if (blkdev_get_size(fd, &size) == 0) {
	    if (size == 0 && !loop)
	      error(_(
		 "       (could this be the IDE device where you in fact use\n"
		 "       ide-scsi so that sr0 or sda or so is needed?)"));

	    if (size && size <= 2)
	      error(_(
		  "       (aren't you trying to mount an extended partition,\n"
		  "       instead of some logical partition inside?)"));

	    close(fd);
	  }
	}
	error(_(
		"       In some cases useful info is found in syslog - try\n"
		"       dmesg | tail  or so\n"));
      }
      break;
    }
    case EMFILE:
      error (_("mount table full")); break;
    case EIO:
      error (_("mount: %s: can't read superblock"), spec); break;
    case ENODEV:
    {
      int pfs = known_fstype_in_procfs(types);

      if (pfs == 1 || !strcmp(types, "guess"))
        error(_("mount: %s: unknown device"), spec);
      else if (pfs == 0) {
	char *lowtype, *p;
	int u;

	error (_("mount: unknown filesystem type '%s'"), types);

	/* maybe this loser asked for FAT or ISO9660 or isofs */
	lowtype = xstrdup(types);
	u = 0;
	for(p=lowtype; *p; p++) {
	  if(tolower(*p) != *p) {
	    *p = tolower(*p);
	    u++;
	  }
	}
	if (u && known_fstype_in_procfs(lowtype) == 1)
	  error (_("mount: probably you meant %s"), lowtype);
	else if (!strncmp(lowtype, "iso", 3) &&
			known_fstype_in_procfs("iso9660") == 1)
	  error (_("mount: maybe you meant 'iso9660'?"));
	else if (!strncmp(lowtype, "fat", 3) &&
			known_fstype_in_procfs("vfat") == 1)
	  error (_("mount: maybe you meant 'vfat'?"));
	free(lowtype);
      } else
	error (_("mount: %s has wrong device number or fs type %s not supported"),
	       spec, types);
      break;
    }
    case ENOTBLK:
      if (opt_nofail)
        goto out;
      if (stat (spec, &statbuf)) /* strange ... */
	error (_("mount: %s is not a block device, and stat fails?"), spec);
      else if (S_ISBLK(statbuf.st_mode))
        error (_("mount: the kernel does not recognize %s as a block device\n"
	       "       (maybe `modprobe driver'?)"), spec);
      else if (S_ISREG(statbuf.st_mode))
	error (_("mount: %s is not a block device (maybe try `-o loop'?)"),
		 spec);
      else
	error (_("mount: %s is not a block device"), spec);
      break;
    case ENXIO:
      if (opt_nofail)
        goto out;
      error (_("mount: %s is not a valid block device"), spec); break;
    case EACCES:  /* pre-linux 1.1.38, 1.1.41 and later */
    case EROFS:   /* linux 1.1.38 and later */
    { char *bd = (loop ? "" : _("block device "));
      if (ro || (flags & MS_RDONLY)) {
          error (_("mount: cannot mount %s%s read-only"),
		 bd, spec);
          break;
      } else if (readwrite) {
	  error (_("mount: %s%s is write-protected but explicit `-w' flag given"),
		 bd, spec);
	  break;
      } else if (flags & MS_REMOUNT) {
	  error (_("mount: cannot remount %s%s read-write, is write-protected"),
		 bd, spec);
	  break;
      } else {
	 opts = opts0;
	 types = types0;

         if (opts) {
	     char *opts2 = append_opt(xstrdup(opts), "ro", NULL);
	     my_free(opts1);
	     opts = opts1 = opts2;
         } else
             opts = "ro";
	 if (types && !strcmp(types, "guess"))
	     types = 0;
         error (_("mount: %s%s is write-protected, mounting read-only"),
		bd, spec0);
	 res = try_mount_one (spec0, node0, types, opts, freq, pass, 1);
	 goto out;
      }
      break;
    }
    case ENOMEDIUM:
      error(_("mount: no medium found on %s"), spec);
      break;
    default:
      error ("mount: %s", strerror (mnt_err)); break;
    }
  }
  res = EX_FAIL;

 out:

#if defined(HAVE_LIBSELINUX) && defined(HAVE_SECURITY_GET_INITIAL_CONTEXT)
  if (res != EX_FAIL && verbose && is_selinux_enabled() > 0) {
      security_context_t raw = NULL, def = NULL;

      if (getfilecon(node, &raw) > 0 &&
		     security_get_initial_context("file", &def) == 0) {

	  if (!selinux_file_context_cmp(raw, def))
	      printf(_("mount: %s does not contain SELinux labels.\n"
                   "       You just mounted an file system that supports labels which does not\n"
                   "       contain labels, onto an SELinux box. It is likely that confined\n"
                   "       applications will generate AVC messages and not be allowed access to\n"
                   "       this file system.  For more details see restorecon(8) and mount(8).\n"),
                   node);
      }
      freecon(raw);
      freecon(def);
  }
#endif

  my_free(mount_opts);
  my_free(extra_opts);
  my_free(spec1);
  my_free(node1);
  my_free(opts1);
  my_free(types1);

  return res;
}