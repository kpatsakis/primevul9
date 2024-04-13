queue_run(uschar *start_id, uschar *stop_id, BOOL recurse)
{
BOOL force_delivery = f.queue_run_force || deliver_selectstring != NULL ||
  deliver_selectstring_sender != NULL;
const pcre *selectstring_regex = NULL;
const pcre *selectstring_regex_sender = NULL;
uschar *log_detail = NULL;
int subcount = 0;
int i;
uschar subdirs[64];

/* Cancel any specific queue domains. Turn off the flag that causes SMTP
deliveries not to happen, unless doing a 2-stage queue run, when the SMTP flag
gets set. Save the queue_runner's pid and the flag that indicates any
deliveries run directly from this process. Deliveries that are run by handing
on TCP/IP channels have queue_run_pid set, but not queue_running. */

queue_domains = NULL;
queue_smtp_domains = NULL;
f.queue_smtp = f.queue_2stage;

queue_run_pid = getpid();
f.queue_running = TRUE;

/* Log the true start of a queue run, and fancy options */

if (!recurse)
  {
  uschar extras[8];
  uschar *p = extras;

  if (f.queue_2stage) *p++ = 'q';
  if (f.queue_run_first_delivery) *p++ = 'i';
  if (f.queue_run_force) *p++ = 'f';
  if (f.deliver_force_thaw) *p++ = 'f';
  if (f.queue_run_local) *p++ = 'l';
  *p = 0;

  p = big_buffer;
  p += sprintf(CS p, "pid=%d", (int)queue_run_pid);

  if (extras[0] != 0)
    p += sprintf(CS p, " -q%s", extras);

  if (deliver_selectstring)
    p += sprintf(CS p, " -R%s %s", f.deliver_selectstring_regex? "r" : "",
      deliver_selectstring);

  if (deliver_selectstring_sender)
    p += sprintf(CS p, " -S%s %s", f.deliver_selectstring_sender_regex? "r" : "",
      deliver_selectstring_sender);

  log_detail = string_copy(big_buffer);
  if (*queue_name)
    log_write(L_queue_run, LOG_MAIN, "Start '%s' queue run: %s",
      queue_name, log_detail);
  else
    log_write(L_queue_run, LOG_MAIN, "Start queue run: %s", log_detail);
  }

/* If deliver_selectstring is a regex, compile it. */

if (deliver_selectstring && f.deliver_selectstring_regex)
  selectstring_regex = regex_must_compile(deliver_selectstring, TRUE, FALSE);

if (deliver_selectstring_sender && f.deliver_selectstring_sender_regex)
  selectstring_regex_sender =
    regex_must_compile(deliver_selectstring_sender, TRUE, FALSE);

/* If the spool is split into subdirectories, we want to process it one
directory at a time, so as to spread out the directory scanning and the
delivering when there are lots of messages involved, except when
queue_run_in_order is set.

In the random order case, this loop runs once for the main directory (handling
any messages therein), and then repeats for any subdirectories that were found.
When the first argument of queue_get_spool_list() is 0, it scans the top
directory, fills in subdirs, and sets subcount. The order of the directories is
then randomized after the first time through, before they are scanned in
subsequent iterations.

When the first argument of queue_get_spool_list() is -1 (for queue_run_in_
order), it scans all directories and makes a single message list. */

for (i = queue_run_in_order ? -1 : 0;
     i <= (queue_run_in_order ? -1 : subcount);
     i++)
  {
  queue_filename * fq;
  void *reset_point1 = store_get(0);

  DEBUG(D_queue_run)
    {
    if (i == 0)
      debug_printf("queue running main directory\n");
    else if (i == -1)
      debug_printf("queue running combined directories\n");
    else
      debug_printf("queue running subdirectory '%c'\n", subdirs[i]);
    }

  for (fq = queue_get_spool_list(i, subdirs, &subcount, !queue_run_in_order);
       fq;
       fq = fq->next)
    {
    pid_t pid;
    int status;
    int pfd[2];
    struct stat statbuf;
    uschar buffer[256];

    /* Unless deliveries are forced, if deliver_queue_load_max is non-negative,
    check that the load average is low enough to permit deliveries. */

    if (!f.queue_run_force && deliver_queue_load_max >= 0)
      if ((load_average = os_getloadavg()) > deliver_queue_load_max)
        {
        log_write(L_queue_run, LOG_MAIN, "Abandon queue run: %s (load %.2f, max %.2f)",
          log_detail,
          (double)load_average/1000.0,
          (double)deliver_queue_load_max/1000.0);
        i = subcount;                 /* Don't process other directories */
        break;
        }
      else
        DEBUG(D_load) debug_printf("load average = %.2f max = %.2f\n",
          (double)load_average/1000.0,
          (double)deliver_queue_load_max/1000.0);

    /* Skip this message unless it's within the ID limits */

    if (stop_id && Ustrncmp(fq->text, stop_id, MESSAGE_ID_LENGTH) > 0)
      continue;
    if (start_id && Ustrncmp(fq->text, start_id, MESSAGE_ID_LENGTH) < 0)
      continue;

    /* Check that the message still exists */

    message_subdir[0] = fq->dir_uschar;
    if (Ustat(spool_fname(US"input", message_subdir, fq->text, US""), &statbuf) < 0)
      continue;

    /* There are some tests that require the reading of the header file. Ensure
    the store used is scavenged afterwards so that this process doesn't keep
    growing its store. We have to read the header file again when actually
    delivering, but it's cheaper than forking a delivery process for each
    message when many are not going to be delivered. */

    if (deliver_selectstring || deliver_selectstring_sender ||
        f.queue_run_first_delivery)
      {
      BOOL wanted = TRUE;
      BOOL orig_dont_deliver = f.dont_deliver;
      void *reset_point2 = store_get(0);

      /* Restore the original setting of dont_deliver after reading the header,
      so that a setting for a particular message doesn't force it for any that
      follow. If the message is chosen for delivery, the header is read again
      in the deliver_message() function, in a subprocess. */

      if (spool_read_header(fq->text, FALSE, TRUE) != spool_read_OK) continue;
      f.dont_deliver = orig_dont_deliver;

      /* Now decide if we want to deliver this message. As we have read the
      header file, we might as well do the freeze test now, and save forking
      another process. */

      if (f.deliver_freeze && !f.deliver_force_thaw)
        {
        log_write(L_skip_delivery, LOG_MAIN, "Message is frozen");
        wanted = FALSE;
        }

      /* Check first_delivery in the case when there are no message logs. */

      else if (f.queue_run_first_delivery && !f.deliver_firsttime)
        {
        DEBUG(D_queue_run) debug_printf("%s: not first delivery\n", fq->text);
        wanted = FALSE;
        }

      /* Check for a matching address if deliver_selectstring[_sender] is set.
      If so, we do a fully delivery - don't want to omit other addresses since
      their routing might trigger re-writing etc. */

      /* Sender matching */

      else if (  deliver_selectstring_sender
	      && !(f.deliver_selectstring_sender_regex
		  ? (pcre_exec(selectstring_regex_sender, NULL,
		      CS sender_address, Ustrlen(sender_address), 0, PCRE_EOPT,
		      NULL, 0) >= 0)
		  : (strstric(sender_address, deliver_selectstring_sender, FALSE)
		      != NULL)
	      )   )
        {
        DEBUG(D_queue_run) debug_printf("%s: sender address did not match %s\n",
          fq->text, deliver_selectstring_sender);
        wanted = FALSE;
        }

      /* Recipient matching */

      else if (deliver_selectstring)
        {
        int i;
        for (i = 0; i < recipients_count; i++)
          {
          uschar *address = recipients_list[i].address;
          if (  (f.deliver_selectstring_regex
		? (pcre_exec(selectstring_regex, NULL, CS address,
		     Ustrlen(address), 0, PCRE_EOPT, NULL, 0) >= 0)
                : (strstric(address, deliver_selectstring, FALSE) != NULL)
		)
             && tree_search(tree_nonrecipients, address) == NULL
	     )
            break;
          }

        if (i >= recipients_count)
          {
          DEBUG(D_queue_run)
            debug_printf("%s: no recipient address matched %s\n",
              fq->text, deliver_selectstring);
          wanted = FALSE;
          }
        }

      /* Recover store used when reading the header */

      spool_clear_header_globals();
      store_reset(reset_point2);
      if (!wanted) continue;      /* With next message */
      }

    /* OK, got a message we want to deliver. Create a pipe which will
    serve as a means of detecting when all the processes created by the
    delivery process are finished. This is relevant when the delivery
    process passes one or more SMTP channels on to its own children. The
    pipe gets passed down; by reading on it here we detect when the last
    descendent dies by the unblocking of the read. It's a pity that for
    most of the time the pipe isn't used, but creating a pipe should be
    pretty cheap. */

    if (pipe(pfd) < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "failed to create pipe in queue "
        "runner process %d: %s", queue_run_pid, strerror(errno));
    queue_run_pipe = pfd[pipe_write];  /* To ensure it gets passed on. */

    /* Make sure it isn't stdin. This seems unlikely, but just to be on the
    safe side... */

    if (queue_run_pipe == 0)
      {
      queue_run_pipe = dup(queue_run_pipe);
      (void)close(0);
      }

    /* Before forking to deliver the message, ensure any open and cached
    lookup files or databases are closed. Otherwise, closing in the subprocess
    can make the next subprocess have problems. There won't often be anything
    open here, but it is possible (e.g. if spool_directory is an expanded
    string). A single call before this loop would probably suffice, but just in
    case expansions get inserted at some point, I've taken the heavy-handed
    approach. When nothing is open, the call should be cheap. */

    search_tidyup();

    /* Now deliver the message; get the id by cutting the -H off the file
    name. The return of the process is zero if a delivery was attempted. */

    set_process_info("running queue: %s", fq->text);
    fq->text[SPOOL_NAME_LENGTH-2] = 0;
    if ((pid = fork()) == 0)
      {
      int rc;
      if (f.running_in_test_harness) millisleep(100);
      (void)close(pfd[pipe_read]);
      rc = deliver_message(fq->text, force_delivery, FALSE);
      _exit(rc == DELIVER_NOT_ATTEMPTED);
      }
    if (pid < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "fork of delivery process from "
        "queue runner %d failed\n", queue_run_pid);

    /* Close the writing end of the synchronizing pipe in this process,
    then wait for the first level process to terminate. */

    (void)close(pfd[pipe_write]);
    set_process_info("running queue: waiting for %s (%d)", fq->text, pid);
    while (wait(&status) != pid);

    /* A zero return means a delivery was attempted; turn off the force flag
    for any subsequent calls unless queue_force is set. */

    if ((status & 0xffff) == 0) force_delivery = f.queue_run_force;

    /* If the process crashed, tell somebody */

    else if ((status & 0x00ff) != 0)
      log_write(0, LOG_MAIN|LOG_PANIC,
        "queue run: process %d crashed with signal %d while delivering %s",
        (int)pid, status & 0x00ff, fq->text);

    /* Before continuing, wait till the pipe gets closed at the far end. This
    tells us that any children created by the delivery to re-use any SMTP
    channels have all finished. Since no process actually writes to the pipe,
    the mere fact that read() unblocks is enough. */

    set_process_info("running queue: waiting for children of %d", pid);
    if ((status = read(pfd[pipe_read], buffer, sizeof(buffer))) != 0)
      log_write(0, LOG_MAIN|LOG_PANIC, "queue run: %s on pipe",
		status > 0 ? "unexpected data" : "error");
    (void)close(pfd[pipe_read]);
    set_process_info("running queue");

    /* If we are in the test harness, and this is not the first of a 2-stage
    queue run, update fudged queue times. */

    if (f.running_in_test_harness && !f.queue_2stage)
      {
      uschar *fqtnext = Ustrchr(fudged_queue_times, '/');
      if (fqtnext != NULL) fudged_queue_times = fqtnext + 1;
      }
    }                                  /* End loop for list of messages */

  tree_nonrecipients = NULL;
  store_reset(reset_point1);           /* Scavenge list of messages */

  /* If this was the first time through for random order processing, and
  sub-directories have been found, randomize their order if necessary. */

  if (i == 0 && subcount > 1 && !queue_run_in_order)
    {
    int j, r;
    for (j = 1; j <= subcount; j++)
      if ((r = random_number(100)) >= 50)
        {
        int k = (r % subcount) + 1;
        int x = subdirs[j];
        subdirs[j] = subdirs[k];
        subdirs[k] = x;
        }
    }
  }                                    /* End loop for multiple directories */

/* If queue_2stage is true, we do it all again, with the 2stage flag
turned off. */

if (f.queue_2stage)
  {
  f.queue_2stage = FALSE;
  queue_run(start_id, stop_id, TRUE);
  }

/* At top level, log the end of the run. */

if (!recurse)
  if (*queue_name)
    log_write(L_queue_run, LOG_MAIN, "End '%s' queue run: %s",
      queue_name, log_detail);
  else
    log_write(L_queue_run, LOG_MAIN, "End queue run: %s", log_detail);
}