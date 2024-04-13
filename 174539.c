par_read_pipe(int poffset, BOOL eop)
{
host_item *h;
pardata *p = parlist + poffset;
address_item *addrlist = p->addrlist;
address_item *addr = p->addr;
pid_t pid = p->pid;
int fd = p->fd;

uschar *msg = p->msg;
BOOL done = p->done;

/* Loop through all items, reading from the pipe when necessary. The pipe
used to be non-blocking. But I do not see a reason for using non-blocking I/O
here, as the preceding select() tells us, if data is available for reading.

A read() on a "selected" handle should never block, but(!) it may return
less data then we expected. (The buffer size we pass to read() shouldn't be
understood as a "request", but as a "limit".)

Each separate item is written to the pipe in a timely manner. But, especially for
larger items, the read(2) may already return partial data from the write(2).

The write is atomic mostly (depending on the amount written), but atomic does
not imply "all or noting", it just is "not intermixed" with other writes on the
same channel (pipe).

*/

DEBUG(D_deliver) debug_printf("reading pipe for subprocess %d (%s)\n",
  (int)p->pid, eop? "ended" : "not ended yet");

while (!done)
  {
  retry_item *r, **rp;
  uschar pipeheader[PIPE_HEADER_SIZE+1];
  uschar *id = &pipeheader[0];
  uschar *subid = &pipeheader[1];
  uschar *ptr = big_buffer;
  size_t required = PIPE_HEADER_SIZE; /* first the pipehaeder, later the data */
  ssize_t got;

  DEBUG(D_deliver) debug_printf(
    "expect %lu bytes (pipeheader) from tpt process %d\n", (u_long)required, pid);

  /* We require(!) all the PIPE_HEADER_SIZE bytes here, as we know,
  they're written in a timely manner, so waiting for the write shouldn't hurt a lot.
  If we get less, we can assume the subprocess do be done and do not expect any further
  information from it. */

  if ((got = readn(fd, pipeheader, required)) != required)
    {
    msg = string_sprintf("got " SSIZE_T_FMT " of %d bytes (pipeheader) "
      "from transport process %d for transport %s",
      got, PIPE_HEADER_SIZE, pid, addr->transport->driver_name);
    done = TRUE;
    break;
    }

  pipeheader[PIPE_HEADER_SIZE] = '\0';
  DEBUG(D_deliver)
    debug_printf("got %ld bytes (pipeheader) from transport process %d\n",
      (long) got, pid);

  {
  /* If we can't decode the pipeheader, the subprocess seems to have a
  problem, we do not expect any furher information from it. */
  char *endc;
  required = Ustrtol(pipeheader+2, &endc, 10);
  if (*endc)
    {
    msg = string_sprintf("failed to read pipe "
      "from transport process %d for transport %s: error decoding size from header",
      pid, addr->transport->driver_name);
    done = TRUE;
    break;
    }
  }

  DEBUG(D_deliver)
    debug_printf("expect %lu bytes (pipedata) from transport process %d\n",
      (u_long)required, pid);

  /* Same as above, the transport process will write the bytes announced
  in a timely manner, so we can just wait for the bytes, getting less than expected
  is considered a problem of the subprocess, we do not expect anything else from it. */
  if ((got = readn(fd, big_buffer, required)) != required)
    {
    msg = string_sprintf("got only " SSIZE_T_FMT " of " SIZE_T_FMT
      " bytes (pipedata) from transport process %d for transport %s",
      got, required, pid, addr->transport->driver_name);
    done = TRUE;
    break;
    }

  /* Handle each possible type of item, assuming the complete item is
  available in store. */

  switch (*id)
    {
    /* Host items exist only if any hosts were marked unusable. Match
    up by checking the IP address. */

    case 'H':
      for (h = addrlist->host_list; h; h = h->next)
	{
	if (!h->address || Ustrcmp(h->address, ptr+2) != 0) continue;
	h->status = ptr[0];
	h->why = ptr[1];
	}
      ptr += 2;
      while (*ptr++);
      break;

    /* Retry items are sent in a preceding R item for each address. This is
    kept separate to keep each message short enough to guarantee it won't
    be split in the pipe. Hopefully, in the majority of cases, there won't in
    fact be any retry items at all.

    The complete set of retry items might include an item to delete a
    routing retry if there was a previous routing delay. However, routing
    retries are also used when a remote transport identifies an address error.
    In that case, there may also be an "add" item for the same key. Arrange
    that a "delete" item is dropped in favour of an "add" item. */

    case 'R':
      if (!addr) goto ADDR_MISMATCH;

      DEBUG(D_deliver|D_retry)
	debug_printf("reading retry information for %s from subprocess\n",
	  ptr+1);

      /* Cut out any "delete" items on the list. */

      for (rp = &addr->retries; (r = *rp); rp = &r->next)
	if (Ustrcmp(r->key, ptr+1) == 0)           /* Found item with same key */
	  {
	  if (!(r->flags & rf_delete)) break;	   /* It was not "delete" */
	  *rp = r->next;                           /* Excise a delete item */
	  DEBUG(D_deliver|D_retry)
	    debug_printf("  existing delete item dropped\n");
	  }

      /* We want to add a delete item only if there is no non-delete item;
      however we still have to step ptr through the data. */

      if (!r || !(*ptr & rf_delete))
	{
	r = store_get(sizeof(retry_item));
	r->next = addr->retries;
	addr->retries = r;
	r->flags = *ptr++;
	r->key = string_copy(ptr);
	while (*ptr++);
	memcpy(&r->basic_errno, ptr, sizeof(r->basic_errno));
	ptr += sizeof(r->basic_errno);
	memcpy(&r->more_errno, ptr, sizeof(r->more_errno));
	ptr += sizeof(r->more_errno);
	r->message = *ptr ? string_copy(ptr) : NULL;
	DEBUG(D_deliver|D_retry) debug_printf("  added %s item\n",
	    r->flags & rf_delete ? "delete" : "retry");
	}

      else
	{
	DEBUG(D_deliver|D_retry)
	  debug_printf("  delete item not added: non-delete item exists\n");
	ptr++;
	while(*ptr++);
	ptr += sizeof(r->basic_errno) + sizeof(r->more_errno);
	}

      while(*ptr++);
      break;

    /* Put the amount of data written into the parlist block */

    case 'S':
      memcpy(&(p->transport_count), ptr, sizeof(transport_count));
      ptr += sizeof(transport_count);
      break;

    /* Address items are in the order of items on the address chain. We
    remember the current address value in case this function is called
    several times to empty the pipe in stages. Information about delivery
    over TLS is sent in a preceding X item for each address. We don't put
    it in with the other info, in order to keep each message short enough to
    guarantee it won't be split in the pipe. */

#ifdef SUPPORT_TLS
    case 'X':
      if (!addr) goto ADDR_MISMATCH;          /* Below, in 'A' handler */
      switch (*subid)
	{
	case '1':
	  addr->cipher = NULL;
	  addr->peerdn = NULL;

	  if (*ptr)
	    addr->cipher = string_copy(ptr);
	  while (*ptr++);
	  if (*ptr)
	    addr->peerdn = string_copy(ptr);
	  break;

	case '2':
	  if (*ptr)
	    (void) tls_import_cert(ptr, &addr->peercert);
	  else
	    addr->peercert = NULL;
	  break;

	case '3':
	  if (*ptr)
	    (void) tls_import_cert(ptr, &addr->ourcert);
	  else
	    addr->ourcert = NULL;
	  break;

# ifndef DISABLE_OCSP
	case '4':
	  addr->ocsp = *ptr ? *ptr - '0' : OCSP_NOT_REQ;
	  break;
# endif
	}
      while (*ptr++);
      break;
#endif	/*SUPPORT_TLS*/

    case 'C':	/* client authenticator information */
      switch (*subid)
	{
	case '1': addr->authenticator = *ptr ? string_copy(ptr) : NULL; break;
	case '2': addr->auth_id = *ptr ? string_copy(ptr) : NULL;	break;
	case '3': addr->auth_sndr = *ptr ? string_copy(ptr) : NULL;	break;
	}
      while (*ptr++);
      break;

#ifndef DISABLE_PRDR
    case 'P':
      setflag(addr, af_prdr_used);
      break;
#endif

    case 'K':
      setflag(addr, af_chunking_used);
      break;

    case 'T':
      setflag(addr, af_tcp_fastopen_conn);
      if (*subid > '0') setflag(addr, af_tcp_fastopen);
      break;

    case 'D':
      if (!addr) goto ADDR_MISMATCH;
      memcpy(&(addr->dsn_aware), ptr, sizeof(addr->dsn_aware));
      ptr += sizeof(addr->dsn_aware);
      DEBUG(D_deliver) debug_printf("DSN read: addr->dsn_aware = %d\n", addr->dsn_aware);
      break;

    case 'A':
      if (!addr)
	{
	ADDR_MISMATCH:
	msg = string_sprintf("address count mismatch for data read from pipe "
	  "for transport process %d for transport %s", pid,
	    addrlist->transport->driver_name);
	done = TRUE;
	break;
	}

      switch (*subid)
	{
  #ifdef SUPPORT_SOCKS
	case '2':	/* proxy information; must arrive before A0 and applies to that addr XXX oops*/
	  proxy_session = TRUE;	/*XXX should this be cleared somewhere? */
	  if (*ptr == 0)
	    ptr++;
	  else
	    {
	    proxy_local_address = string_copy(ptr);
	    while(*ptr++);
	    memcpy(&proxy_local_port, ptr, sizeof(proxy_local_port));
	    ptr += sizeof(proxy_local_port);
	    }
	  break;
  #endif

  #ifdef EXPERIMENTAL_DSN_INFO
	case '1':	/* must arrive before A0, and applies to that addr */
			/* Two strings: smtp_greeting and helo_response */
	  addr->smtp_greeting = string_copy(ptr);
	  while(*ptr++);
	  addr->helo_response = string_copy(ptr);
	  while(*ptr++);
	  break;
  #endif

	case '0':
	  DEBUG(D_deliver) debug_printf("A0 %s tret %d\n", addr->address, *ptr);
	  addr->transport_return = *ptr++;
	  addr->special_action = *ptr++;
	  memcpy(&addr->basic_errno, ptr, sizeof(addr->basic_errno));
	  ptr += sizeof(addr->basic_errno);
	  memcpy(&addr->more_errno, ptr, sizeof(addr->more_errno));
	  ptr += sizeof(addr->more_errno);
	  memcpy(&addr->delivery_usec, ptr, sizeof(addr->delivery_usec));
	  ptr += sizeof(addr->delivery_usec);
	  memcpy(&addr->flags, ptr, sizeof(addr->flags));
	  ptr += sizeof(addr->flags);
	  addr->message = *ptr ? string_copy(ptr) : NULL;
	  while(*ptr++);
	  addr->user_message = *ptr ? string_copy(ptr) : NULL;
	  while(*ptr++);

	  /* Always two strings for host information, followed by the port number and DNSSEC mark */

	  if (*ptr)
	    {
	    h = store_get(sizeof(host_item));
	    h->name = string_copy(ptr);
	    while (*ptr++);
	    h->address = string_copy(ptr);
	    while(*ptr++);
	    memcpy(&h->port, ptr, sizeof(h->port));
	    ptr += sizeof(h->port);
	    h->dnssec = *ptr == '2' ? DS_YES
		      : *ptr == '1' ? DS_NO
		      : DS_UNK;
	    ptr++;
	    addr->host_used = h;
	    }
	  else ptr++;

	  /* Finished with this address */

	  addr = addr->next;
	  break;
	}
      break;

    /* Local interface address/port */
    case 'I':
      if (*ptr) sending_ip_address = string_copy(ptr);
      while (*ptr++) ;
      if (*ptr) sending_port = atoi(CS ptr);
      while (*ptr++) ;
      break;

    /* Z marks the logical end of the data. It is followed by '0' if
    continue_transport was NULL at the end of transporting, otherwise '1'.
    We need to know when it becomes NULL during a delivery down a passed SMTP
    channel so that we don't try to pass anything more down it. Of course, for
    most normal messages it will remain NULL all the time. */

    case 'Z':
      if (*ptr == '0')
	{
	continue_transport = NULL;
	continue_hostname = NULL;
	}
      done = TRUE;
      DEBUG(D_deliver) debug_printf("Z0%c item read\n", *ptr);
      break;

    /* Anything else is a disaster. */

    default:
      msg = string_sprintf("malformed data (%d) read from pipe for transport "
	"process %d for transport %s", ptr[-1], pid,
	  addr->transport->driver_name);
      done = TRUE;
      break;
    }
  }

/* The done flag is inspected externally, to determine whether or not to
call the function again when the process finishes. */

p->done = done;

/* If the process hadn't finished, and we haven't seen the end of the data
or if we suffered a disaster, update the rest of the state, and return FALSE to
indicate "not finished". */

if (!eop && !done)
  {
  p->addr = addr;
  p->msg = msg;
  return FALSE;
  }

/* Close our end of the pipe, to prevent deadlock if the far end is still
pushing stuff into it. */

(void)close(fd);
p->fd = -1;

/* If we have finished without error, but haven't had data for every address,
something is wrong. */

if (!msg && addr)
  msg = string_sprintf("insufficient address data read from pipe "
    "for transport process %d for transport %s", pid,
      addr->transport->driver_name);

/* If an error message is set, something has gone wrong in getting back
the delivery data. Put the message into each address and freeze it. */

if (msg)
  for (addr = addrlist; addr; addr = addr->next)
    {
    addr->transport_return = DEFER;
    addr->special_action = SPECIAL_FREEZE;
    addr->message = msg;
    log_write(0, LOG_MAIN|LOG_PANIC, "Delivery status for %s: %s\n", addr->address, addr->message);
    }

/* Return TRUE to indicate we have got all we need from this process, even
if it hasn't actually finished yet. */

return TRUE;
}