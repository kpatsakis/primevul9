do_remote_deliveries(BOOL fallback)
{
int parmax;
int delivery_count;
int poffset;

parcount = 0;    /* Number of executing subprocesses */

/* When sending down an existing channel, only do one delivery at a time.
We use a local variable (parmax) to hold the maximum number of processes;
this gets reduced from remote_max_parallel if we can't create enough pipes. */

if (continue_transport) remote_max_parallel = 1;
parmax = remote_max_parallel;

/* If the data for keeping a list of processes hasn't yet been
set up, do so. */

if (!parlist)
  {
  parlist = store_get(remote_max_parallel * sizeof(pardata));
  for (poffset = 0; poffset < remote_max_parallel; poffset++)
    parlist[poffset].pid = 0;
  }

/* Now loop for each remote delivery */

for (delivery_count = 0; addr_remote; delivery_count++)
  {
  pid_t pid;
  uid_t uid;
  gid_t gid;
  int pfd[2];
  int address_count = 1;
  int address_count_max;
  BOOL multi_domain;
  BOOL use_initgroups;
  BOOL pipe_done = FALSE;
  transport_instance *tp;
  address_item **anchor = &addr_remote;
  address_item *addr = addr_remote;
  address_item *last = addr;
  address_item *next;
  uschar * panicmsg;
  uschar * serialize_key = NULL;

  /* Pull the first address right off the list. */

  addr_remote = addr->next;
  addr->next = NULL;

  DEBUG(D_deliver|D_transport)
    debug_printf("--------> %s <--------\n", addr->address);

  /* If no transport has been set, there has been a big screw-up somewhere. */

  if (!(tp = addr->transport))
    {
    disable_logging = FALSE;  /* Jic */
    panicmsg = US"No transport set by router";
    goto panic_continue;
    }

  /* Check that this base address hasn't previously been delivered to this
  transport. The check is necessary at this point to handle homonymic addresses
  correctly in cases where the pattern of redirection changes between delivery
  attempts. Non-homonymic previous delivery is detected earlier, at routing
  time. */

  if (previously_transported(addr, FALSE)) continue;

  /* Force failure if the message is too big. */

  if (tp->message_size_limit)
    {
    int rc = check_message_size(tp, addr);
    if (rc != OK)
      {
      addr->transport_return = rc;
      remote_post_process(addr, LOG_MAIN, NULL, fallback);
      continue;
      }
    }

  /* Get the flag which specifies whether the transport can handle different
  domains that nevertheless resolve to the same set of hosts. If it needs
  expanding, get variables set: $address_data, $domain_data, $localpart_data,
  $host, $host_address, $host_port. */
  if (tp->expand_multi_domain)
    deliver_set_expansions(addr);

  if (exp_bool(addr, US"transport", tp->name, D_transport,
		US"multi_domain", tp->multi_domain, tp->expand_multi_domain,
		&multi_domain) != OK)
    {
    deliver_set_expansions(NULL);
    panicmsg = addr->message;
    goto panic_continue;
    }

  /* Get the maximum it can handle in one envelope, with zero meaning
  unlimited, which is forced for the MUA wrapper case. */

  address_count_max = tp->max_addresses;
  if (address_count_max == 0 || mua_wrapper) address_count_max = 999999;


  /************************************************************************/
  /*****    This is slightly experimental code, but should be safe.   *****/

  /* The address_count_max value is the maximum number of addresses that the
  transport can send in one envelope. However, the transport must be capable of
  dealing with any number of addresses. If the number it gets exceeds its
  envelope limitation, it must send multiple copies of the message. This can be
  done over a single connection for SMTP, so uses less resources than making
  multiple connections. On the other hand, if remote_max_parallel is greater
  than one, it is perhaps a good idea to use parallel processing to move the
  message faster, even if that results in multiple simultaneous connections to
  the same host.

  How can we come to some compromise between these two ideals? What we do is to
  limit the number of addresses passed to a single instance of a transport to
  the greater of (a) its address limit (rcpt_max for SMTP) and (b) the total
  number of addresses routed to remote transports divided by
  remote_max_parallel. For example, if the message has 100 remote recipients,
  remote max parallel is 2, and rcpt_max is 10, we'd never send more than 50 at
  once. But if rcpt_max is 100, we could send up to 100.

  Of course, not all the remotely addresses in a message are going to go to the
  same set of hosts (except in smarthost configurations), so this is just a
  heuristic way of dividing up the work.

  Furthermore (1), because this may not be wanted in some cases, and also to
  cope with really pathological cases, there is also a limit to the number of
  messages that are sent over one connection. This is the same limit that is
  used when sending several different messages over the same connection.
  Continue_sequence is set when in this situation, to the number sent so
  far, including this message.

  Furthermore (2), when somebody explicitly sets the maximum value to 1, it
  is probably because they are using VERP, in which case they want to pass only
  one address at a time to the transport, in order to be able to use
  $local_part and $domain in constructing a new return path. We could test for
  the use of these variables, but as it is so likely they will be used when the
  maximum is 1, we don't bother. Just leave the value alone. */

  if (  address_count_max != 1
     && address_count_max < remote_delivery_count/remote_max_parallel
     )
    {
    int new_max = remote_delivery_count/remote_max_parallel;
    int message_max = tp->connection_max_messages;
    if (connection_max_messages >= 0) message_max = connection_max_messages;
    message_max -= continue_sequence - 1;
    if (message_max > 0 && new_max > address_count_max * message_max)
      new_max = address_count_max * message_max;
    address_count_max = new_max;
    }

  /************************************************************************/


  /* Pick off all addresses which have the same transport, errors address,
  destination, and extra headers. In some cases they point to the same host
  list, but we also need to check for identical host lists generated from
  entirely different domains. The host list pointers can be NULL in the case
  where the hosts are defined in the transport. There is also a configured
  maximum limit of addresses that can be handled at once (see comments above
  for how it is computed).
  If the transport does not handle multiple domains, enforce that also,
  and if it might need a per-address check for this, re-evaluate it.
  */

  while ((next = *anchor) && address_count < address_count_max)
    {
    BOOL md;
    if (  (multi_domain || Ustrcmp(next->domain, addr->domain) == 0)
       && tp == next->transport
       && same_hosts(next->host_list, addr->host_list)
       && same_strings(next->prop.errors_address, addr->prop.errors_address)
       && same_headers(next->prop.extra_headers, addr->prop.extra_headers)
       && same_ugid(tp, next, addr)
       && (  next->prop.remove_headers == addr->prop.remove_headers
	  || (  next->prop.remove_headers
	     && addr->prop.remove_headers
	     && Ustrcmp(next->prop.remove_headers, addr->prop.remove_headers) == 0
	  )  )
       && (  !multi_domain
	  || (  (
		(void)(!tp->expand_multi_domain || ((void)deliver_set_expansions(next), 1)),
	        exp_bool(addr,
		    US"transport", next->transport->name, D_transport,
		    US"multi_domain", next->transport->multi_domain,
		    next->transport->expand_multi_domain, &md) == OK
	        )
	     && md
       )  )  )
      {
      *anchor = next->next;
      next->next = NULL;
      next->first = addr;  /* remember top one (for retry processing) */
      last->next = next;
      last = next;
      address_count++;
      }
    else anchor = &(next->next);
    deliver_set_expansions(NULL);
    }

  /* If we are acting as an MUA wrapper, all addresses must go in a single
  transaction. If not, put them back on the chain and yield FALSE. */

  if (mua_wrapper && addr_remote)
    {
    last->next = addr_remote;
    addr_remote = addr;
    return FALSE;
    }

  /* If the transport is limited for parallellism, enforce that here.
  The hints DB entry is decremented in par_reduce(), when we reap the
  transport process. */

  if (tpt_parallel_check(tp, addr, &serialize_key))
    if ((panicmsg = expand_string_message))
      goto panic_continue;
    else
      continue;			/* Loop for the next set of addresses. */

  /* Set up the expansion variables for this set of addresses */

  deliver_set_expansions(addr);

  /* Ensure any transport-set auth info is fresh */
  addr->authenticator = addr->auth_id = addr->auth_sndr = NULL;

  /* Compute the return path, expanding a new one if required. The old one
  must be set first, as it might be referred to in the expansion. */

  if(addr->prop.errors_address)
    return_path = addr->prop.errors_address;
#ifdef EXPERIMENTAL_SRS
  else if(addr->prop.srs_sender)
    return_path = addr->prop.srs_sender;
#endif
  else
    return_path = sender_address;

  if (tp->return_path)
    {
    uschar *new_return_path = expand_string(tp->return_path);
    if (new_return_path)
      return_path = new_return_path;
    else if (!expand_string_forcedfail)
      {
      panicmsg = string_sprintf("Failed to expand return path \"%s\": %s",
	tp->return_path, expand_string_message);
      goto enq_continue;
      }
    }

  /* Find the uid, gid, and use_initgroups setting for this transport. Failure
  logs and sets up error messages, so we just post-process and continue with
  the next address. */

  if (!findugid(addr, tp, &uid, &gid, &use_initgroups))
    {
    panicmsg = NULL;
    goto enq_continue;
    }

  /* If this transport has a setup function, call it now so that it gets
  run in this process and not in any subprocess. That way, the results of
  any setup that are retained by the transport can be reusable. One of the
  things the setup does is to set the fallback host lists in the addresses.
  That is why it is called at this point, before the continue delivery
  processing, because that might use the fallback hosts. */

  if (tp->setup)
    (void)((tp->setup)(addr->transport, addr, NULL, uid, gid, NULL));

  /* If we have a connection still open from a verify stage (lazy-close)
  treat it as if it is a continued connection (apart from the counter used
  for the log line mark). */

  if (cutthrough.fd >= 0 && cutthrough.callout_hold_only)
    {
    DEBUG(D_deliver)
      debug_printf("lazy-callout-close: have conn still open from verification\n");
    continue_transport = cutthrough.transport;
    continue_hostname = string_copy(cutthrough.host.name);
    continue_host_address = string_copy(cutthrough.host.address);
    continue_sequence = 1;
    sending_ip_address = cutthrough.snd_ip;
    sending_port = cutthrough.snd_port;
    smtp_peer_options = cutthrough.peer_options;
    }

  /* If this is a run to continue delivery down an already-established
  channel, check that this set of addresses matches the transport and
  the channel. If it does not, defer the addresses. If a host list exists,
  we must check that the continue host is on the list. Otherwise, the
  host is set in the transport. */

  continue_more = FALSE;           /* In case got set for the last lot */
  if (continue_transport)
    {
    BOOL ok = Ustrcmp(continue_transport, tp->name) == 0;

    /* If the transport is about to override the host list do not check
    it here but take the cost of running the transport process to discover
    if the continued_hostname connection is suitable.  This is a layering
    violation which is unfortunate as it requires we haul in the smtp
    include file. */

    if (ok)
      {
      smtp_transport_options_block * ob;

      if (  !(  Ustrcmp(tp->info->driver_name, "smtp") == 0
	     && (ob = (smtp_transport_options_block *)tp->options_block)
	     && ob->hosts_override && ob->hosts
	     )
	 && addr->host_list
	 )
	{
	host_item * h;
	ok = FALSE;
	for (h = addr->host_list; h; h = h->next)
	  if (Ustrcmp(h->name, continue_hostname) == 0)
  /*XXX should also check port here */
	    { ok = TRUE; break; }
	}
      }

    /* Addresses not suitable; defer or queue for fallback hosts (which
    might be the continue host) and skip to next address. */

    if (!ok)
      {
      DEBUG(D_deliver) debug_printf("not suitable for continue_transport (%s)\n",
	Ustrcmp(continue_transport, tp->name) != 0
	? string_sprintf("tpt %s vs %s", continue_transport, tp->name)
	: string_sprintf("no host matching %s", continue_hostname));
      if (serialize_key) enq_end(serialize_key);

      if (addr->fallback_hosts && !fallback)
        {
	for (next = addr; ; next = next->next)
          {
          next->host_list = next->fallback_hosts;
          DEBUG(D_deliver) debug_printf("%s queued for fallback host(s)\n", next->address);
          if (!next->next) break;
          }
        next->next = addr_fallback;
        addr_fallback = addr;
        }

      else
	{
	for (next = addr; ; next = next->next)
	  {
	  DEBUG(D_deliver) debug_printf(" %s to def list\n", next->address);
          if (!next->next) break;
	  }
	next->next = addr_defer;
	addr_defer = addr;
	}

      continue;
      }

    /* Set a flag indicating whether there are further addresses that list
    the continued host. This tells the transport to leave the channel open,
    but not to pass it to another delivery process. We'd like to do that
    for non-continue_transport cases too but the knowlege of which host is
    connected to is too hard to manage.  Perhaps we need a finer-grain
    interface to the transport. */

    for (next = addr_remote; next && !continue_more; next = next->next)
      {
      host_item *h;
      for (h = next->host_list; h; h = h->next)
        if (Ustrcmp(h->name, continue_hostname) == 0)
          { continue_more = TRUE; break; }
      }
    }

  /* The transports set up the process info themselves as they may connect
  to more than one remote machine. They also have to set up the filter
  arguments, if required, so that the host name and address are available
  for expansion. */

  transport_filter_argv = NULL;

  /* Create the pipe for inter-process communication. If pipe creation
  fails, it is probably because the value of remote_max_parallel is so
  large that too many file descriptors for pipes have been created. Arrange
  to wait for a process to finish, and then try again. If we still can't
  create a pipe when all processes have finished, break the retry loop. */

  while (!pipe_done)
    {
    if (pipe(pfd) == 0) pipe_done = TRUE;
      else if (parcount > 0) parmax = parcount;
        else break;

    /* We need to make the reading end of the pipe non-blocking. There are
    two different options for this. Exim is cunningly (I hope!) coded so
    that it can use either of them, though it prefers O_NONBLOCK, which
    distinguishes between EOF and no-more-data. */

/* The data appears in a timely manner and we already did a select on
all pipes, so I do not see a reason to use non-blocking IO here

#ifdef O_NONBLOCK
    (void)fcntl(pfd[pipe_read], F_SETFL, O_NONBLOCK);
#else
    (void)fcntl(pfd[pipe_read], F_SETFL, O_NDELAY);
#endif
*/

    /* If the maximum number of subprocesses already exist, wait for a process
    to finish. If we ran out of file descriptors, parmax will have been reduced
    from its initial value of remote_max_parallel. */

    par_reduce(parmax - 1, fallback);
    }

  /* If we failed to create a pipe and there were no processes to wait
  for, we have to give up on this one. Do this outside the above loop
  so that we can continue the main loop. */

  if (!pipe_done)
    {
    panicmsg = string_sprintf("unable to create pipe: %s", strerror(errno));
    goto enq_continue;
    }

  /* Find a free slot in the pardata list. Must do this after the possible
  waiting for processes to finish, because a terminating process will free
  up a slot. */

  for (poffset = 0; poffset < remote_max_parallel; poffset++)
    if (parlist[poffset].pid == 0)
      break;

  /* If there isn't one, there has been a horrible disaster. */

  if (poffset >= remote_max_parallel)
    {
    (void)close(pfd[pipe_write]);
    (void)close(pfd[pipe_read]);
    panicmsg = US"Unexpectedly no free subprocess slot";
    goto enq_continue;
    }

  /* Now fork a subprocess to do the remote delivery, but before doing so,
  ensure that any cached resources are released so as not to interfere with
  what happens in the subprocess. */

  search_tidyup();


  if ((pid = fork()) == 0)
    {
    int fd = pfd[pipe_write];
    host_item *h;
    DEBUG(D_deliver) debug_selector |= D_pid;  // hs12

    /* Setting this global in the subprocess means we need never clear it */
    transport_name = tp->name;

    /* There are weird circumstances in which logging is disabled */
    disable_logging = tp->disable_logging;

    /* Show pids on debug output if parallelism possible */

    if (parmax > 1 && (parcount > 0 || addr_remote))
      {
      DEBUG(D_any|D_v) debug_selector |= D_pid;
      DEBUG(D_deliver) debug_printf("Remote delivery process started\n");
      }

    /* Reset the random number generator, so different processes don't all
    have the same sequence. In the test harness we want different, but
    predictable settings for each delivery process, so do something explicit
    here rather they rely on the fixed reset in the random number function. */

    random_seed = running_in_test_harness ? 42 + 2*delivery_count : 0;

    /* Set close-on-exec on the pipe so that it doesn't get passed on to
    a new process that may be forked to do another delivery down the same
    SMTP connection. */

    (void)fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

    /* Close open file descriptors for the pipes of other processes
    that are running in parallel. */

    for (poffset = 0; poffset < remote_max_parallel; poffset++)
      if (parlist[poffset].pid != 0) (void)close(parlist[poffset].fd);

    /* This process has inherited a copy of the file descriptor
    for the data file, but its file pointer is shared with all the
    other processes running in parallel. Therefore, we have to re-open
    the file in order to get a new file descriptor with its own
    file pointer. We don't need to lock it, as the lock is held by
    the parent process. There doesn't seem to be any way of doing
    a dup-with-new-file-pointer. */

    (void)close(deliver_datafile);
    {
    uschar * fname = spool_fname(US"input", message_subdir, message_id, US"-D");

    if ((deliver_datafile = Uopen(fname,
#ifdef O_CLOEXEC
					O_CLOEXEC |
#endif
					O_RDWR | O_APPEND, 0)) < 0)
      log_write(0, LOG_MAIN|LOG_PANIC_DIE, "Failed to reopen %s for remote "
        "parallel delivery: %s", fname, strerror(errno));
    }

    /* Set the close-on-exec flag */
#ifndef O_CLOEXEC
    (void)fcntl(deliver_datafile, F_SETFD, fcntl(deliver_datafile, F_GETFD) |
      FD_CLOEXEC);
#endif

    /* Set the uid/gid of this process; bombs out on failure. */

    exim_setugid(uid, gid, use_initgroups,
      string_sprintf("remote delivery to %s with transport=%s",
        addr->address, tp->name));

    /* Close the unwanted half of this process' pipe, set the process state,
    and run the transport. Afterwards, transport_count will contain the number
    of bytes written. */

    (void)close(pfd[pipe_read]);
    set_process_info("delivering %s using %s", message_id, tp->name);
    debug_print_string(tp->debug_string);
    if (!(tp->info->code)(addr->transport, addr)) replicate_status(addr);

    set_process_info("delivering %s (just run %s for %s%s in subprocess)",
      message_id, tp->name, addr->address, addr->next ? ", ..." : "");

    /* Ensure any cached resources that we used are now released */

    search_tidyup();

    /* Pass the result back down the pipe. This is a lot more information
    than is needed for a local delivery. We have to send back the error
    status for each address, the usability status for each host that is
    flagged as unusable, and all the retry items. When TLS is in use, we
    send also the cipher and peerdn information. Each type of information
    is flagged by an identifying byte, and is then in a fixed format (with
    strings terminated by zeros), and there is a final terminator at the
    end. The host information and retry information is all attached to
    the first address, so that gets sent at the start. */

    /* Host unusability information: for most success cases this will
    be null. */

    for (h = addr->host_list; h; h = h->next)
      {
      if (!h->address || h->status < hstatus_unusable) continue;
      sprintf(CS big_buffer, "%c%c%s", h->status, h->why, h->address);
      rmt_dlv_checked_write(fd, 'H', '0', big_buffer, Ustrlen(big_buffer+2) + 3);
      }

    /* The number of bytes written. This is the same for each address. Even
    if we sent several copies of the message down the same connection, the
    size of each one is the same, and it's that value we have got because
    transport_count gets reset before calling transport_write_message(). */

    memcpy(big_buffer, &transport_count, sizeof(transport_count));
    rmt_dlv_checked_write(fd, 'S', '0', big_buffer, sizeof(transport_count));

    /* Information about what happened to each address. Four item types are
    used: an optional 'X' item first, for TLS information, then an optional "C"
    item for any client-auth info followed by 'R' items for any retry settings,
    and finally an 'A' item for the remaining data. */

    for(; addr; addr = addr->next)
      {
      uschar *ptr;
      retry_item *r;

      /* The certificate verification status goes into the flags */
      if (tls_out.certificate_verified) setflag(addr, af_cert_verified);
#ifdef SUPPORT_DANE
      if (tls_out.dane_verified)        setflag(addr, af_dane_verified);
#endif

      /* Use an X item only if there's something to send */
#ifdef SUPPORT_TLS
      if (addr->cipher)
        {
        ptr = big_buffer + sprintf(CS big_buffer, "%.128s", addr->cipher) + 1;
        if (!addr->peerdn)
	  *ptr++ = 0;
	else
          ptr += sprintf(CS ptr, "%.512s", addr->peerdn) + 1;

        rmt_dlv_checked_write(fd, 'X', '1', big_buffer, ptr - big_buffer);
        }
      else if (continue_proxy_cipher)
	{
        ptr = big_buffer + sprintf(CS big_buffer, "%.128s", continue_proxy_cipher) + 1;
	*ptr++ = 0;
        rmt_dlv_checked_write(fd, 'X', '1', big_buffer, ptr - big_buffer);
	}

      if (addr->peercert)
	{
        ptr = big_buffer;
	if (!tls_export_cert(ptr, big_buffer_size-2, addr->peercert))
	  while(*ptr++);
	else
	  *ptr++ = 0;
        rmt_dlv_checked_write(fd, 'X', '2', big_buffer, ptr - big_buffer);
	}
      if (addr->ourcert)
	{
        ptr = big_buffer;
	if (!tls_export_cert(ptr, big_buffer_size-2, addr->ourcert))
	  while(*ptr++);
	else
	  *ptr++ = 0;
        rmt_dlv_checked_write(fd, 'X', '3', big_buffer, ptr - big_buffer);
	}
# ifndef DISABLE_OCSP
      if (addr->ocsp > OCSP_NOT_REQ)
	{
	ptr = big_buffer + sprintf(CS big_buffer, "%c", addr->ocsp + '0') + 1;
        rmt_dlv_checked_write(fd, 'X', '4', big_buffer, ptr - big_buffer);
	}
# endif
#endif	/*SUPPORT_TLS*/

      if (client_authenticator)
        {
	ptr = big_buffer + sprintf(CS big_buffer, "%.64s", client_authenticator) + 1;
        rmt_dlv_checked_write(fd, 'C', '1', big_buffer, ptr - big_buffer);
	}
      if (client_authenticated_id)
        {
        ptr = big_buffer + sprintf(CS big_buffer, "%.64s", client_authenticated_id) + 1;
        rmt_dlv_checked_write(fd, 'C', '2', big_buffer, ptr - big_buffer);
	}
      if (client_authenticated_sender)
        {
        ptr = big_buffer + sprintf(CS big_buffer, "%.64s", client_authenticated_sender) + 1;
        rmt_dlv_checked_write(fd, 'C', '3', big_buffer, ptr - big_buffer);
	}

#ifndef DISABLE_PRDR
      if (testflag(addr, af_prdr_used))
	rmt_dlv_checked_write(fd, 'P', '0', NULL, 0);
#endif

      if (testflag(addr, af_chunking_used))
	rmt_dlv_checked_write(fd, 'K', '0', NULL, 0);

      if (testflag(addr, af_tcp_fastopen_conn))
	rmt_dlv_checked_write(fd, 'T',
	  testflag(addr, af_tcp_fastopen) ? '1' : '0', NULL, 0);

      memcpy(big_buffer, &addr->dsn_aware, sizeof(addr->dsn_aware));
      rmt_dlv_checked_write(fd, 'D', '0', big_buffer, sizeof(addr->dsn_aware));

      /* Retry information: for most success cases this will be null. */

      for (r = addr->retries; r; r = r->next)
        {
        sprintf(CS big_buffer, "%c%.500s", r->flags, r->key);
        ptr = big_buffer + Ustrlen(big_buffer+2) + 3;
        memcpy(ptr, &r->basic_errno, sizeof(r->basic_errno));
        ptr += sizeof(r->basic_errno);
        memcpy(ptr, &r->more_errno, sizeof(r->more_errno));
        ptr += sizeof(r->more_errno);
        if (!r->message) *ptr++ = 0; else
          {
          sprintf(CS ptr, "%.512s", r->message);
          while(*ptr++);
          }
        rmt_dlv_checked_write(fd, 'R', '0', big_buffer, ptr - big_buffer);
        }

#ifdef SUPPORT_SOCKS
      if (LOGGING(proxy) && proxy_session)
	{
	ptr = big_buffer;
	if (proxy_local_address)
	  {
	  DEBUG(D_deliver) debug_printf("proxy_local_address '%s'\n", proxy_local_address);
	  ptr = big_buffer + sprintf(CS ptr, "%.128s", proxy_local_address) + 1;
	  DEBUG(D_deliver) debug_printf("proxy_local_port %d\n", proxy_local_port);
	  memcpy(ptr, &proxy_local_port, sizeof(proxy_local_port));
	  ptr += sizeof(proxy_local_port);
	  }
	else
	  *ptr++ = '\0';
	rmt_dlv_checked_write(fd, 'A', '2', big_buffer, ptr - big_buffer);
	}
#endif

#ifdef EXPERIMENTAL_DSN_INFO
/*um, are they really per-addr?  Other per-conn stuff is not (auth, tls).  But host_used is! */
      if (addr->smtp_greeting)
	{
	DEBUG(D_deliver) debug_printf("smtp_greeting '%s'\n", addr->smtp_greeting);
	ptr = big_buffer + sprintf(CS big_buffer, "%.128s", addr->smtp_greeting) + 1;
	if (addr->helo_response)
	  {
	  DEBUG(D_deliver) debug_printf("helo_response '%s'\n", addr->helo_response);
	  ptr += sprintf(CS ptr, "%.128s", addr->helo_response) + 1;
	  }
	else
	  *ptr++ = '\0';
        rmt_dlv_checked_write(fd, 'A', '1', big_buffer, ptr - big_buffer);
	}
#endif

      /* The rest of the information goes in an 'A0' item. */

      sprintf(CS big_buffer, "%c%c", addr->transport_return, addr->special_action);
      ptr = big_buffer + 2;
      memcpy(ptr, &addr->basic_errno, sizeof(addr->basic_errno));
      ptr += sizeof(addr->basic_errno);
      memcpy(ptr, &addr->more_errno, sizeof(addr->more_errno));
      ptr += sizeof(addr->more_errno);
      memcpy(ptr, &addr->delivery_usec, sizeof(addr->delivery_usec));
      ptr += sizeof(addr->delivery_usec);
      memcpy(ptr, &addr->flags, sizeof(addr->flags));
      ptr += sizeof(addr->flags);

      if (!addr->message) *ptr++ = 0; else
        ptr += sprintf(CS ptr, "%.1024s", addr->message) + 1;

      if (!addr->user_message) *ptr++ = 0; else
        ptr += sprintf(CS ptr, "%.1024s", addr->user_message) + 1;

      if (!addr->host_used) *ptr++ = 0; else
        {
        ptr += sprintf(CS ptr, "%.256s", addr->host_used->name) + 1;
        ptr += sprintf(CS ptr, "%.64s", addr->host_used->address) + 1;
        memcpy(ptr, &addr->host_used->port, sizeof(addr->host_used->port));
        ptr += sizeof(addr->host_used->port);

        /* DNS lookup status */
	*ptr++ = addr->host_used->dnssec==DS_YES ? '2'
	       : addr->host_used->dnssec==DS_NO ? '1' : '0';

        }
      rmt_dlv_checked_write(fd, 'A', '0', big_buffer, ptr - big_buffer);
      }

    /* Local interface address/port */
#ifdef EXPERIMENTAL_DSN_INFO
    if (sending_ip_address)
#else
    if (LOGGING(incoming_interface) && sending_ip_address)
#endif
      {
      uschar * ptr;
      ptr = big_buffer + sprintf(CS big_buffer, "%.128s", sending_ip_address) + 1;
      ptr += sprintf(CS ptr, "%d", sending_port) + 1;
      rmt_dlv_checked_write(fd, 'I', '0', big_buffer, ptr - big_buffer);
      }

    /* Add termination flag, close the pipe, and that's it. The character
    after 'Z' indicates whether continue_transport is now NULL or not.
    A change from non-NULL to NULL indicates a problem with a continuing
    connection. */

    big_buffer[0] = continue_transport ? '1' : '0';
    rmt_dlv_checked_write(fd, 'Z', '0', big_buffer, 1);
    (void)close(fd);
    exit(EXIT_SUCCESS);
    }

  /* Back in the mainline: close the unwanted half of the pipe. */

  (void)close(pfd[pipe_write]);

  /* If we have a connection still open from a verify stage (lazy-close)
  release its TLS library context (if any) as responsibility was passed to
  the delivery child process. */

  if (cutthrough.fd >= 0 && cutthrough.callout_hold_only)
    {
#ifdef SUPPORT_TLS
    tls_close(FALSE, TLS_NO_SHUTDOWN);
#endif
    (void) close(cutthrough.fd);
    release_cutthrough_connection(US"passed to transport proc");
    }

  /* Fork failed; defer with error message */

  if (pid == -1)
    {
    (void)close(pfd[pipe_read]);
    panicmsg = string_sprintf("fork failed for remote delivery to %s: %s",
        addr->domain, strerror(errno));
    goto enq_continue;
    }

  /* Fork succeeded; increment the count, and remember relevant data for
  when the process finishes. */

  parcount++;
  parlist[poffset].addrlist = parlist[poffset].addr = addr;
  parlist[poffset].pid = pid;
  parlist[poffset].fd = pfd[pipe_read];
  parlist[poffset].done = FALSE;
  parlist[poffset].msg = NULL;
  parlist[poffset].return_path = return_path;

  /* If the process we've just started is sending a message down an existing
  channel, wait for it now. This ensures that only one such process runs at
  once, whatever the value of remote_max parallel. Otherwise, we might try to
  send two or more messages simultaneously down the same channel. This could
  happen if there are different domains that include the same host in otherwise
  different host lists.

  Also, if the transport closes down the channel, this information gets back
  (continue_transport gets set to NULL) before we consider any other addresses
  in this message. */

  if (continue_transport) par_reduce(0, fallback);

  /* Otherwise, if we are running in the test harness, wait a bit, to let the
  newly created process get going before we create another process. This should
  ensure repeatability in the tests. We only need to wait a tad. */

  else if (running_in_test_harness) millisleep(500);

  continue;

enq_continue:
  if (serialize_key) enq_end(serialize_key);
panic_continue:
  remote_post_process(addr, LOG_MAIN|LOG_PANIC, panicmsg, fallback);
  continue;
  }

/* Reached the end of the list of addresses. Wait for all the subprocesses that
are still running and post-process their addresses. */

par_reduce(0, fallback);
return TRUE;
}