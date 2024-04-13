deliver_local(address_item *addr, BOOL shadowing)
{
BOOL use_initgroups;
uid_t uid;
gid_t gid;
int status, len, rc;
int pfd[2];
pid_t pid;
uschar *working_directory;
address_item *addr2;
transport_instance *tp = addr->transport;

/* Set up the return path from the errors or sender address. If the transport
has its own return path setting, expand it and replace the existing value. */

if(addr->prop.errors_address)
  return_path = addr->prop.errors_address;
#ifdef EXPERIMENTAL_SRS
else if (addr->prop.srs_sender)
  return_path = addr->prop.srs_sender;
#endif
else
  return_path = sender_address;

if (tp->return_path)
  {
  uschar *new_return_path = expand_string(tp->return_path);
  if (!new_return_path)
    {
    if (!expand_string_forcedfail)
      {
      common_error(TRUE, addr, ERRNO_EXPANDFAIL,
        US"Failed to expand return path \"%s\" in %s transport: %s",
        tp->return_path, tp->name, expand_string_message);
      return;
      }
    }
  else return_path = new_return_path;
  }

/* For local deliveries, one at a time, the value used for logging can just be
set directly, once and for all. */

used_return_path = return_path;

/* Sort out the uid, gid, and initgroups flag. If an error occurs, the message
gets put into the address(es), and the expansions are unset, so we can just
return. */

if (!findugid(addr, tp, &uid, &gid, &use_initgroups)) return;

/* See if either the transport or the address specifies a home directory. A
home directory set in the address may already be expanded; a flag is set to
indicate that. In other cases we must expand it. */

if (  (deliver_home = tp->home_dir)		/* Set in transport, or */
   || (  (deliver_home = addr->home_dir)	/* Set in address and */
      && !testflag(addr, af_home_expanded)	/*   not expanded */
   )  )
  {
  uschar *rawhome = deliver_home;
  deliver_home = NULL;                      /* in case it contains $home */
  if (!(deliver_home = expand_string(rawhome)))
    {
    common_error(TRUE, addr, ERRNO_EXPANDFAIL, US"home directory \"%s\" failed "
      "to expand for %s transport: %s", rawhome, tp->name,
      expand_string_message);
    return;
    }
  if (*deliver_home != '/')
    {
    common_error(TRUE, addr, ERRNO_NOTABSOLUTE, US"home directory path \"%s\" "
      "is not absolute for %s transport", deliver_home, tp->name);
    return;
    }
  }

/* See if either the transport or the address specifies a current directory,
and if so, expand it. If nothing is set, use the home directory, unless it is
also unset in which case use "/", which is assumed to be a directory to which
all users have access. It is necessary to be in a visible directory for some
operating systems when running pipes, as some commands (e.g. "rm" under Solaris
2.5) require this. */

working_directory = tp->current_dir ? tp->current_dir : addr->current_dir;
if (working_directory)
  {
  uschar *raw = working_directory;
  if (!(working_directory = expand_string(raw)))
    {
    common_error(TRUE, addr, ERRNO_EXPANDFAIL, US"current directory \"%s\" "
      "failed to expand for %s transport: %s", raw, tp->name,
      expand_string_message);
    return;
    }
  if (*working_directory != '/')
    {
    common_error(TRUE, addr, ERRNO_NOTABSOLUTE, US"current directory path "
      "\"%s\" is not absolute for %s transport", working_directory, tp->name);
    return;
    }
  }
else working_directory = deliver_home ? deliver_home : US"/";

/* If one of the return_output flags is set on the transport, create and open a
file in the message log directory for the transport to write its output onto.
This is mainly used by pipe transports. The file needs to be unique to the
address. This feature is not available for shadow transports. */

if (  !shadowing
   && (  tp->return_output || tp->return_fail_output
      || tp->log_output || tp->log_fail_output || tp->log_defer_output
   )  )
  {
  uschar * error;

  addr->return_filename =
    spool_fname(US"msglog", message_subdir, message_id,
      string_sprintf("-%d-%d", getpid(), return_count++));

  if ((addr->return_file = open_msglog_file(addr->return_filename, 0400, &error)) < 0)
    {
    common_error(TRUE, addr, errno, US"Unable to %s file for %s transport "
      "to return message: %s", error, tp->name, strerror(errno));
    return;
    }
  }

/* Create the pipe for inter-process communication. */

if (pipe(pfd) != 0)
  {
  common_error(TRUE, addr, ERRNO_PIPEFAIL, US"Creation of pipe failed: %s",
    strerror(errno));
  return;
  }

/* Now fork the process to do the real work in the subprocess, but first
ensure that all cached resources are freed so that the subprocess starts with
a clean slate and doesn't interfere with the parent process. */

search_tidyup();

if ((pid = fork()) == 0)
  {
  BOOL replicate = TRUE;

  /* Prevent core dumps, as we don't want them in users' home directories.
  HP-UX doesn't have RLIMIT_CORE; I don't know how to do this in that
  system. Some experimental/developing systems (e.g. GNU/Hurd) may define
  RLIMIT_CORE but not support it in setrlimit(). For such systems, do not
  complain if the error is "not supported".

  There are two scenarios where changing the max limit has an effect.  In one,
  the user is using a .forward and invoking a command of their choice via pipe;
  for these, we do need the max limit to be 0 unless the admin chooses to
  permit an increased limit.  In the other, the command is invoked directly by
  the transport and is under administrator control, thus being able to raise
  the limit aids in debugging.  So there's no general always-right answer.

  Thus we inhibit core-dumps completely but let individual transports, while
  still root, re-raise the limits back up to aid debugging.  We make the
  default be no core-dumps -- few enough people can use core dumps in
  diagnosis that it's reasonable to make them something that has to be explicitly requested.
  */

#ifdef RLIMIT_CORE
  struct rlimit rl;
  rl.rlim_cur = 0;
  rl.rlim_max = 0;
  if (setrlimit(RLIMIT_CORE, &rl) < 0)
    {
# ifdef SETRLIMIT_NOT_SUPPORTED
    if (errno != ENOSYS && errno != ENOTSUP)
# endif
      log_write(0, LOG_MAIN|LOG_PANIC, "setrlimit(RLIMIT_CORE) failed: %s",
        strerror(errno));
    }
#endif

  /* Reset the random number generator, so different processes don't all
  have the same sequence. */

  random_seed = 0;

  /* If the transport has a setup entry, call this first, while still
  privileged. (Appendfile uses this to expand quota, for example, while
  able to read private files.) */

  if (addr->transport->setup)
    switch((addr->transport->setup)(addr->transport, addr, NULL, uid, gid,
           &(addr->message)))
      {
      case DEFER:
	addr->transport_return = DEFER;
	goto PASS_BACK;

      case FAIL:
	addr->transport_return = PANIC;
	goto PASS_BACK;
      }

  /* Ignore SIGINT and SIGTERM during delivery. Also ignore SIGUSR1, as
  when the process becomes unprivileged, it won't be able to write to the
  process log. SIGHUP is ignored throughout exim, except when it is being
  run as a daemon. */

  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGUSR1, SIG_IGN);

  /* Close the unwanted half of the pipe, and set close-on-exec for the other
  half - for transports that exec things (e.g. pipe). Then set the required
  gid/uid. */

  (void)close(pfd[pipe_read]);
  (void)fcntl(pfd[pipe_write], F_SETFD, fcntl(pfd[pipe_write], F_GETFD) |
    FD_CLOEXEC);
  exim_setugid(uid, gid, use_initgroups,
    string_sprintf("local delivery to %s <%s> transport=%s", addr->local_part,
      addr->address, addr->transport->name));

  DEBUG(D_deliver)
    {
    address_item *batched;
    debug_printf("  home=%s current=%s\n", deliver_home, working_directory);
    for (batched = addr->next; batched; batched = batched->next)
      debug_printf("additional batched address: %s\n", batched->address);
    }

  /* Set an appropriate working directory. */

  if (Uchdir(working_directory) < 0)
    {
    addr->transport_return = DEFER;
    addr->basic_errno = errno;
    addr->message = string_sprintf("failed to chdir to %s", working_directory);
    }

  /* If successful, call the transport */

  else
    {
    BOOL ok = TRUE;
    set_process_info("delivering %s to %s using %s", message_id,
     addr->local_part, addr->transport->name);

    /* Setting this global in the subprocess means we need never clear it */
    transport_name = addr->transport->name;

    /* If a transport filter has been specified, set up its argument list.
    Any errors will get put into the address, and FALSE yielded. */

    if (addr->transport->filter_command)
      {
      ok = transport_set_up_command(&transport_filter_argv,
        addr->transport->filter_command,
        TRUE, PANIC, addr, US"transport filter", NULL);
      transport_filter_timeout = addr->transport->filter_timeout;
      }
    else transport_filter_argv = NULL;

    if (ok)
      {
      debug_print_string(addr->transport->debug_string);
      replicate = !(addr->transport->info->code)(addr->transport, addr);
      }
    }

  /* Pass the results back down the pipe. If necessary, first replicate the
  status in the top address to the others in the batch. The label is the
  subject of a goto when a call to the transport's setup function fails. We
  pass the pointer to the transport back in case it got changed as a result of
  file_format in appendfile. */

  PASS_BACK:

  if (replicate) replicate_status(addr);
  for (addr2 = addr; addr2; addr2 = addr2->next)
    {
    int i;
    int local_part_length = Ustrlen(addr2->local_part);
    uschar *s;
    int ret;

    if(  (ret = write(pfd[pipe_write], &addr2->transport_return, sizeof(int))) != sizeof(int)
      || (ret = write(pfd[pipe_write], &transport_count, sizeof(transport_count))) != sizeof(transport_count)
      || (ret = write(pfd[pipe_write], &addr2->flags, sizeof(addr2->flags))) != sizeof(addr2->flags)
      || (ret = write(pfd[pipe_write], &addr2->basic_errno,    sizeof(int))) != sizeof(int)
      || (ret = write(pfd[pipe_write], &addr2->more_errno,     sizeof(int))) != sizeof(int)
      || (ret = write(pfd[pipe_write], &addr2->delivery_usec,  sizeof(int))) != sizeof(int)
      || (ret = write(pfd[pipe_write], &addr2->special_action, sizeof(int))) != sizeof(int)
      || (ret = write(pfd[pipe_write], &addr2->transport,
        sizeof(transport_instance *))) != sizeof(transport_instance *)

    /* For a file delivery, pass back the local part, in case the original
    was only part of the final delivery path. This gives more complete
    logging. */

      || (testflag(addr2, af_file)
          && (  (ret = write(pfd[pipe_write], &local_part_length, sizeof(int))) != sizeof(int)
             || (ret = write(pfd[pipe_write], addr2->local_part, local_part_length)) != local_part_length
	     )
	 )
      )
      log_write(0, LOG_MAIN|LOG_PANIC, "Failed writing transport results to pipe: %s",
	ret == -1 ? strerror(errno) : "short write");

    /* Now any messages */

    for (i = 0, s = addr2->message; i < 2; i++, s = addr2->user_message)
      {
      int message_length = s ? Ustrlen(s) + 1 : 0;
      if(  (ret = write(pfd[pipe_write], &message_length, sizeof(int))) != sizeof(int)
        || message_length > 0  && (ret = write(pfd[pipe_write], s, message_length)) != message_length
	)
        log_write(0, LOG_MAIN|LOG_PANIC, "Failed writing transport results to pipe: %s",
	  ret == -1 ? strerror(errno) : "short write");
      }
    }

  /* OK, this process is now done. Free any cached resources that it opened,
  and close the pipe we were writing down before exiting. */

  (void)close(pfd[pipe_write]);
  search_tidyup();
  exit(EXIT_SUCCESS);
  }

/* Back in the main process: panic if the fork did not succeed. This seems
better than returning an error - if forking is failing it is probably best
not to try other deliveries for this message. */

if (pid < 0)
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Fork failed for local delivery to %s",
    addr->address);

/* Read the pipe to get the delivery status codes and error messages. Our copy
of the writing end must be closed first, as otherwise read() won't return zero
on an empty pipe. We check that a status exists for each address before
overwriting the address structure. If data is missing, the default DEFER status
will remain. Afterwards, close the reading end. */

(void)close(pfd[pipe_write]);

for (addr2 = addr; addr2; addr2 = addr2->next)
  {
  if ((len = read(pfd[pipe_read], &status, sizeof(int))) > 0)
    {
    int i;
    uschar **sptr;

    addr2->transport_return = status;
    len = read(pfd[pipe_read], &transport_count,
      sizeof(transport_count));
    len = read(pfd[pipe_read], &addr2->flags, sizeof(addr2->flags));
    len = read(pfd[pipe_read], &addr2->basic_errno,    sizeof(int));
    len = read(pfd[pipe_read], &addr2->more_errno,     sizeof(int));
    len = read(pfd[pipe_read], &addr2->delivery_usec,  sizeof(int));
    len = read(pfd[pipe_read], &addr2->special_action, sizeof(int));
    len = read(pfd[pipe_read], &addr2->transport,
      sizeof(transport_instance *));

    if (testflag(addr2, af_file))
      {
      int llen;
      if (  read(pfd[pipe_read], &llen, sizeof(int)) != sizeof(int)
	 || llen > 64*4	/* limit from rfc 5821, times I18N factor */
         )
	{
	log_write(0, LOG_MAIN|LOG_PANIC, "bad local_part length read"
	  " from delivery subprocess");
	break;
	}
      /* sanity-checked llen so disable the Coverity error */
      /* coverity[tainted_data] */
      if (read(pfd[pipe_read], big_buffer, llen) != llen)
	{
	log_write(0, LOG_MAIN|LOG_PANIC, "bad local_part read"
	  " from delivery subprocess");
	break;
	}
      big_buffer[llen] = 0;
      addr2->local_part = string_copy(big_buffer);
      }

    for (i = 0, sptr = &addr2->message; i < 2; i++, sptr = &addr2->user_message)
      {
      int message_length;
      len = read(pfd[pipe_read], &message_length, sizeof(int));
      if (message_length > 0)
        {
        len = read(pfd[pipe_read], big_buffer, message_length);
	big_buffer[big_buffer_size-1] = '\0';		/* guard byte */
        if (len > 0) *sptr = string_copy(big_buffer);
        }
      }
    }

  else
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to read delivery status for %s "
      "from delivery subprocess", addr2->unique);
    break;
    }
  }

(void)close(pfd[pipe_read]);

/* Unless shadowing, write all successful addresses immediately to the journal
file, to ensure they are recorded asap. For homonymic addresses, use the base
address plus the transport name. Failure to write the journal is panic-worthy,
but don't stop, as it may prove possible subsequently to update the spool file
in order to record the delivery. */

if (!shadowing)
  {
  for (addr2 = addr; addr2; addr2 = addr2->next)
    if (addr2->transport_return == OK)
      {
      if (testflag(addr2, af_homonym))
	sprintf(CS big_buffer, "%.500s/%s\n", addr2->unique + 3, tp->name);
      else
	sprintf(CS big_buffer, "%.500s\n", addr2->unique);

      /* In the test harness, wait just a bit to let the subprocess finish off
      any debug output etc first. */

      if (running_in_test_harness) millisleep(300);

      DEBUG(D_deliver) debug_printf("journalling %s", big_buffer);
      len = Ustrlen(big_buffer);
      if (write(journal_fd, big_buffer, len) != len)
	log_write(0, LOG_MAIN|LOG_PANIC, "failed to update journal for %s: %s",
	  big_buffer, strerror(errno));
      }

  /* Ensure the journal file is pushed out to disk. */

  if (EXIMfsync(journal_fd) < 0)
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to fsync journal: %s",
      strerror(errno));
  }

/* Wait for the process to finish. If it terminates with a non-zero code,
freeze the message (except for SIGTERM, SIGKILL and SIGQUIT), but leave the
status values of all the addresses as they are. Take care to handle the case
when the subprocess doesn't seem to exist. This has been seen on one system
when Exim was called from an MUA that set SIGCHLD to SIG_IGN. When that
happens, wait() doesn't recognize the termination of child processes. Exim now
resets SIGCHLD to SIG_DFL, but this code should still be robust. */

while ((rc = wait(&status)) != pid)
  if (rc < 0 && errno == ECHILD)      /* Process has vanished */
    {
    log_write(0, LOG_MAIN, "%s transport process vanished unexpectedly",
      addr->transport->driver_name);
    status = 0;
    break;
    }

if ((status & 0xffff) != 0)
  {
  int msb = (status >> 8) & 255;
  int lsb = status & 255;
  int code = (msb == 0)? (lsb & 0x7f) : msb;
  if (msb != 0 || (code != SIGTERM && code != SIGKILL && code != SIGQUIT))
    addr->special_action = SPECIAL_FREEZE;
  log_write(0, LOG_MAIN|LOG_PANIC, "%s transport process returned non-zero "
    "status 0x%04x: %s %d",
    addr->transport->driver_name,
    status,
    msb == 0 ? "terminated by signal" : "exit code",
    code);
  }

/* If SPECIAL_WARN is set in the top address, send a warning message. */

if (addr->special_action == SPECIAL_WARN && addr->transport->warn_message)
  {
  int fd;
  uschar *warn_message;
  pid_t pid;

  DEBUG(D_deliver) debug_printf("Warning message requested by transport\n");

  if (!(warn_message = expand_string(addr->transport->warn_message)))
    log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand \"%s\" (warning "
      "message for %s transport): %s", addr->transport->warn_message,
      addr->transport->name, expand_string_message);

  else if ((pid = child_open_exim(&fd)) > 0)
    {
    FILE *f = fdopen(fd, "wb");
    if (errors_reply_to && !contains_header(US"Reply-To", warn_message))
      fprintf(f, "Reply-To: %s\n", errors_reply_to);
    fprintf(f, "Auto-Submitted: auto-replied\n");
    if (!contains_header(US"From", warn_message))
      moan_write_from(f);
    fprintf(f, "%s", CS warn_message);

    /* Close and wait for child process to complete, without a timeout. */

    (void)fclose(f);
    (void)child_close(pid, 0);
    }

  addr->special_action = SPECIAL_NONE;
  }
}