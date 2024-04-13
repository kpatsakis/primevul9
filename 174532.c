do_local_deliveries(void)
{
open_db dbblock;
open_db *dbm_file = NULL;
time_t now = time(NULL);

/* Loop until we have exhausted the supply of local deliveries */

while (addr_local)
  {
  struct timeval delivery_start;
  struct timeval deliver_time;
  address_item *addr2, *addr3, *nextaddr;
  int logflags = LOG_MAIN;
  int logchar = dont_deliver? '*' : '=';
  transport_instance *tp;
  uschar * serialize_key = NULL;

  /* Pick the first undelivered address off the chain */

  address_item *addr = addr_local;
  addr_local = addr->next;
  addr->next = NULL;

  DEBUG(D_deliver|D_transport)
    debug_printf("--------> %s <--------\n", addr->address);

  /* An internal disaster if there is no transport. Should not occur! */

  if (!(tp = addr->transport))
    {
    logflags |= LOG_PANIC;
    disable_logging = FALSE;  /* Jic */
    addr->message = addr->router
      ? string_sprintf("No transport set by %s router", addr->router->name)
      : string_sprintf("No transport set by system filter");
    post_process_one(addr, DEFER, logflags, EXIM_DTYPE_TRANSPORT, 0);
    continue;
    }

  /* Check that this base address hasn't previously been delivered to this
  transport. The check is necessary at this point to handle homonymic addresses
  correctly in cases where the pattern of redirection changes between delivery
  attempts. Non-homonymic previous delivery is detected earlier, at routing
  time. */

  if (previously_transported(addr, FALSE)) continue;

  /* There are weird cases where logging is disabled */

  disable_logging = tp->disable_logging;

  /* Check for batched addresses and possible amalgamation. Skip all the work
  if either batch_max <= 1 or there aren't any other addresses for local
  delivery. */

  if (tp->batch_max > 1 && addr_local)
    {
    int batch_count = 1;
    BOOL uses_dom = readconf_depends((driver_instance *)tp, US"domain");
    BOOL uses_lp = (  testflag(addr, af_pfr)
		   && (testflag(addr, af_file) || addr->local_part[0] == '|')
		   )
		|| readconf_depends((driver_instance *)tp, US"local_part");
    uschar *batch_id = NULL;
    address_item **anchor = &addr_local;
    address_item *last = addr;
    address_item *next;

    /* Expand the batch_id string for comparison with other addresses.
    Expansion failure suppresses batching. */

    if (tp->batch_id)
      {
      deliver_set_expansions(addr);
      batch_id = expand_string(tp->batch_id);
      deliver_set_expansions(NULL);
      if (!batch_id)
        {
        log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand batch_id option "
          "in %s transport (%s): %s", tp->name, addr->address,
          expand_string_message);
        batch_count = tp->batch_max;
        }
      }

    /* Until we reach the batch_max limit, pick off addresses which have the
    same characteristics. These are:

      same transport
      not previously delivered (see comment about 50 lines above)
      same local part if the transport's configuration contains $local_part
        or if this is a file or pipe delivery from a redirection
      same domain if the transport's configuration contains $domain
      same errors address
      same additional headers
      same headers to be removed
      same uid/gid for running the transport
      same first host if a host list is set
    */

    while ((next = *anchor) && batch_count < tp->batch_max)
      {
      BOOL ok =
           tp == next->transport
	&& !previously_transported(next, TRUE)
	&& testflag(addr, af_pfr) == testflag(next, af_pfr)
	&& testflag(addr, af_file) == testflag(next, af_file)
	&& (!uses_lp  || Ustrcmp(next->local_part, addr->local_part) == 0)
	&& (!uses_dom || Ustrcmp(next->domain, addr->domain) == 0)
	&& same_strings(next->prop.errors_address, addr->prop.errors_address)
	&& same_headers(next->prop.extra_headers, addr->prop.extra_headers)
	&& same_strings(next->prop.remove_headers, addr->prop.remove_headers)
	&& same_ugid(tp, addr, next)
	&& (  !addr->host_list && !next->host_list
	   ||    addr->host_list
	      && next->host_list
	      && Ustrcmp(addr->host_list->name, next->host_list->name) == 0
	   );

      /* If the transport has a batch_id setting, batch_id will be non-NULL
      from the expansion outside the loop. Expand for this address and compare.
      Expansion failure makes this address ineligible for batching. */

      if (ok && batch_id)
        {
        uschar *bid;
        address_item *save_nextnext = next->next;
        next->next = NULL;            /* Expansion for a single address */
        deliver_set_expansions(next);
        next->next = save_nextnext;
        bid = expand_string(tp->batch_id);
        deliver_set_expansions(NULL);
        if (!bid)
          {
          log_write(0, LOG_MAIN|LOG_PANIC, "Failed to expand batch_id option "
            "in %s transport (%s): %s", tp->name, next->address,
            expand_string_message);
          ok = FALSE;
          }
        else ok = (Ustrcmp(batch_id, bid) == 0);
        }

      /* Take address into batch if OK. */

      if (ok)
        {
        *anchor = next->next;           /* Include the address */
        next->next = NULL;
        last->next = next;
        last = next;
        batch_count++;
        }
      else anchor = &next->next;        /* Skip the address */
      }
    }

  /* We now have one or more addresses that can be delivered in a batch. Check
  whether the transport is prepared to accept a message of this size. If not,
  fail them all forthwith. If the expansion fails, or does not yield an
  integer, defer delivery. */

  if (tp->message_size_limit)
    {
    int rc = check_message_size(tp, addr);
    if (rc != OK)
      {
      replicate_status(addr);
      while (addr)
        {
        addr2 = addr->next;
        post_process_one(addr, rc, logflags, EXIM_DTYPE_TRANSPORT, 0);
        addr = addr2;
        }
      continue;    /* With next batch of addresses */
      }
    }

  /* If we are not running the queue, or if forcing, all deliveries will be
  attempted. Otherwise, we must respect the retry times for each address. Even
  when not doing this, we need to set up the retry key string, and determine
  whether a retry record exists, because after a successful delivery, a delete
  retry item must be set up. Keep the retry database open only for the duration
  of these checks, rather than for all local deliveries, because some local
  deliveries (e.g. to pipes) can take a substantial time. */

  if (!(dbm_file = dbfn_open(US"retry", O_RDONLY, &dbblock, FALSE)))
    {
    DEBUG(D_deliver|D_retry|D_hints_lookup)
      debug_printf("no retry data available\n");
    }

  addr2 = addr;
  addr3 = NULL;
  while (addr2)
    {
    BOOL ok = TRUE;   /* to deliver this address */
    uschar *retry_key;

    /* Set up the retry key to include the domain or not, and change its
    leading character from "R" to "T". Must make a copy before doing this,
    because the old key may be pointed to from a "delete" retry item after
    a routing delay. */

    retry_key = string_copy(
      tp->retry_use_local_part ? addr2->address_retry_key :
        addr2->domain_retry_key);
    *retry_key = 'T';

    /* Inspect the retry data. If there is no hints file, delivery happens. */

    if (dbm_file)
      {
      dbdata_retry *retry_record = dbfn_read(dbm_file, retry_key);

      /* If there is no retry record, delivery happens. If there is,
      remember it exists so it can be deleted after a successful delivery. */

      if (retry_record)
        {
        setflag(addr2, af_lt_retry_exists);

        /* A retry record exists for this address. If queue running and not
        forcing, inspect its contents. If the record is too old, or if its
        retry time has come, or if it has passed its cutoff time, delivery
        will go ahead. */

        DEBUG(D_retry)
          {
          debug_printf("retry record exists: age=%s ",
            readconf_printtime(now - retry_record->time_stamp));
          debug_printf("(max %s)\n", readconf_printtime(retry_data_expire));
          debug_printf("  time to retry = %s expired = %d\n",
            readconf_printtime(retry_record->next_try - now),
            retry_record->expired);
          }

        if (queue_running && !deliver_force)
          {
          ok = (now - retry_record->time_stamp > retry_data_expire)
	    || (now >= retry_record->next_try)
	    || retry_record->expired;

          /* If we haven't reached the retry time, there is one more check
          to do, which is for the ultimate address timeout. */

          if (!ok)
            ok = retry_ultimate_address_timeout(retry_key, addr2->domain,
                retry_record, now);
          }
        }
      else DEBUG(D_retry) debug_printf("no retry record exists\n");
      }

    /* This address is to be delivered. Leave it on the chain. */

    if (ok)
      {
      addr3 = addr2;
      addr2 = addr2->next;
      }

    /* This address is to be deferred. Take it out of the chain, and
    post-process it as complete. Must take it out of the chain first,
    because post processing puts it on another chain. */

    else
      {
      address_item *this = addr2;
      this->message = US"Retry time not yet reached";
      this->basic_errno = ERRNO_LRETRY;
      addr2 = addr3 ? (addr3->next = addr2->next)
		    : (addr = addr2->next);
      post_process_one(this, DEFER, logflags, EXIM_DTYPE_TRANSPORT, 0);
      }
    }

  if (dbm_file) dbfn_close(dbm_file);

  /* If there are no addresses left on the chain, they all deferred. Loop
  for the next set of addresses. */

  if (!addr) continue;

  /* If the transport is limited for parallellism, enforce that here.
  We use a hints DB entry, incremented here and decremented after
  the transport (and any shadow transport) completes. */

  if (tpt_parallel_check(tp, addr, &serialize_key))
    {
    if (expand_string_message)
      {
      logflags |= LOG_PANIC;
      do
	{
	addr = addr->next;
	post_process_one(addr, DEFER, logflags, EXIM_DTYPE_TRANSPORT, 0);
	} while ((addr = addr2));
      }
    continue;			/* Loop for the next set of addresses. */
    }


  /* So, finally, we do have some addresses that can be passed to the
  transport. Before doing so, set up variables that are relevant to a
  single delivery. */

  deliver_set_expansions(addr);

  gettimeofday(&delivery_start, NULL);
  deliver_local(addr, FALSE);
  timesince(&deliver_time, &delivery_start);

  /* If a shadow transport (which must perforce be another local transport), is
  defined, and its condition is met, we must pass the message to the shadow
  too, but only those addresses that succeeded. We do this by making a new
  chain of addresses - also to keep the original chain uncontaminated. We must
  use a chain rather than doing it one by one, because the shadow transport may
  batch.

  NOTE: if the condition fails because of a lookup defer, there is nothing we
  can do! */

  if (  tp->shadow
     && (  !tp->shadow_condition
        || expand_check_condition(tp->shadow_condition, tp->name, US"transport")
     )  )
    {
    transport_instance *stp;
    address_item *shadow_addr = NULL;
    address_item **last = &shadow_addr;

    for (stp = transports; stp; stp = stp->next)
      if (Ustrcmp(stp->name, tp->shadow) == 0) break;

    if (!stp)
      log_write(0, LOG_MAIN|LOG_PANIC, "shadow transport \"%s\" not found ",
        tp->shadow);

    /* Pick off the addresses that have succeeded, and make clones. Put into
    the shadow_message field a pointer to the shadow_message field of the real
    address. */

    else for (addr2 = addr; addr2; addr2 = addr2->next)
      if (addr2->transport_return == OK)
	{
	addr3 = store_get(sizeof(address_item));
	*addr3 = *addr2;
	addr3->next = NULL;
	addr3->shadow_message = US &addr2->shadow_message;
	addr3->transport = stp;
	addr3->transport_return = DEFER;
	addr3->return_filename = NULL;
	addr3->return_file = -1;
	*last = addr3;
	last = &addr3->next;
	}

    /* If we found any addresses to shadow, run the delivery, and stick any
    message back into the shadow_message field in the original. */

    if (shadow_addr)
      {
      int save_count = transport_count;

      DEBUG(D_deliver|D_transport)
        debug_printf(">>>>>>>>>>>>>>>> Shadow delivery >>>>>>>>>>>>>>>>\n");
      deliver_local(shadow_addr, TRUE);

      for(; shadow_addr; shadow_addr = shadow_addr->next)
        {
        int sresult = shadow_addr->transport_return;
        *(uschar **)shadow_addr->shadow_message =
	  sresult == OK
	  ? string_sprintf(" ST=%s", stp->name)
	  : string_sprintf(" ST=%s (%s%s%s)", stp->name,
	      shadow_addr->basic_errno <= 0
	      ? US""
	      : US strerror(shadow_addr->basic_errno),
	      shadow_addr->basic_errno <= 0 || !shadow_addr->message
	      ? US""
	      : US": ",
	      shadow_addr->message
	      ? shadow_addr->message
	      : shadow_addr->basic_errno <= 0
	      ? US"unknown error"
	      : US"");

        DEBUG(D_deliver|D_transport)
          debug_printf("%s shadow transport returned %s for %s\n",
            stp->name,
            sresult == OK ?    "OK" :
            sresult == DEFER ? "DEFER" :
            sresult == FAIL ?  "FAIL" :
            sresult == PANIC ? "PANIC" : "?",
            shadow_addr->address);
        }

      DEBUG(D_deliver|D_transport)
        debug_printf(">>>>>>>>>>>>>>>> End shadow delivery >>>>>>>>>>>>>>>>\n");

      transport_count = save_count;   /* Restore original transport count */
      }
    }

  /* Cancel the expansions that were set up for the delivery. */

  deliver_set_expansions(NULL);

  /* If the transport was parallelism-limited, decrement the hints DB record. */

  if (serialize_key) enq_end(serialize_key);

  /* Now we can process the results of the real transport. We must take each
  address off the chain first, because post_process_one() puts it on another
  chain. */

  for (addr2 = addr; addr2; addr2 = nextaddr)
    {
    int result = addr2->transport_return;
    nextaddr = addr2->next;

    DEBUG(D_deliver|D_transport)
      debug_printf("%s transport returned %s for %s\n",
        tp->name,
        result == OK ?    "OK" :
        result == DEFER ? "DEFER" :
        result == FAIL ?  "FAIL" :
        result == PANIC ? "PANIC" : "?",
        addr2->address);

    /* If there is a retry_record, or if delivery is deferred, build a retry
    item for setting a new retry time or deleting the old retry record from
    the database. These items are handled all together after all addresses
    have been handled (so the database is open just for a short time for
    updating). */

    if (result == DEFER || testflag(addr2, af_lt_retry_exists))
      {
      int flags = result == DEFER ? 0 : rf_delete;
      uschar *retry_key = string_copy(tp->retry_use_local_part
	? addr2->address_retry_key : addr2->domain_retry_key);
      *retry_key = 'T';
      retry_add_item(addr2, retry_key, flags);
      }

    /* Done with this address */

    if (result == OK)
      {
      addr2->more_errno = deliver_time.tv_sec;
      addr2->delivery_usec = deliver_time.tv_usec;
      }
    post_process_one(addr2, result, logflags, EXIM_DTYPE_TRANSPORT, logchar);

    /* If a pipe delivery generated text to be sent back, the result may be
    changed to FAIL, and we must copy this for subsequent addresses in the
    batch. */

    if (addr2->transport_return != result)
      {
      for (addr3 = nextaddr; addr3; addr3 = addr3->next)
        {
        addr3->transport_return = addr2->transport_return;
        addr3->basic_errno = addr2->basic_errno;
        addr3->message = addr2->message;
        }
      result = addr2->transport_return;
      }

    /* Whether or not the result was changed to FAIL, we need to copy the
    return_file value from the first address into all the addresses of the
    batch, so they are all listed in the error message. */

    addr2->return_file = addr->return_file;

    /* Change log character for recording successful deliveries. */

    if (result == OK) logchar = '-';
    }
  }        /* Loop back for next batch of addresses */
}