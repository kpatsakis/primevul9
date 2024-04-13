post_process_one(address_item *addr, int result, int logflags, int driver_type,
  int logchar)
{
uschar *now = tod_stamp(tod_log);
uschar *driver_kind = NULL;
uschar *driver_name = NULL;

DEBUG(D_deliver) debug_printf("post-process %s (%d)\n", addr->address, result);

/* Set up driver kind and name for logging. Disable logging if the router or
transport has disabled it. */

if (driver_type == EXIM_DTYPE_TRANSPORT)
  {
  if (addr->transport)
    {
    driver_name = addr->transport->name;
    driver_kind = US" transport";
    disable_logging = addr->transport->disable_logging;
    }
  else driver_kind = US"transporting";
  }
else if (driver_type == EXIM_DTYPE_ROUTER)
  {
  if (addr->router)
    {
    driver_name = addr->router->name;
    driver_kind = US" router";
    disable_logging = addr->router->disable_logging;
    }
  else driver_kind = US"routing";
  }

/* If there's an error message set, ensure that it contains only printing
characters - it should, but occasionally things slip in and this at least
stops the log format from getting wrecked. We also scan the message for an LDAP
expansion item that has a password setting, and flatten the password. This is a
fudge, but I don't know a cleaner way of doing this. (If the item is badly
malformed, it won't ever have gone near LDAP.) */

if (addr->message)
  {
  const uschar * s = string_printing(addr->message);

  /* deconst cast ok as string_printing known to have alloc'n'copied */
  addr->message = expand_hide_passwords(US s);
  }

/* If we used a transport that has one of the "return_output" options set, and
if it did in fact generate some output, then for return_output we treat the
message as failed if it was not already set that way, so that the output gets
returned to the sender, provided there is a sender to send it to. For
return_fail_output, do this only if the delivery failed. Otherwise we just
unlink the file, and remove the name so that if the delivery failed, we don't
try to send back an empty or unwanted file. The log_output options operate only
on a non-empty file.

In any case, we close the message file, because we cannot afford to leave a
file-descriptor for one address while processing (maybe very many) others. */

if (addr->return_file >= 0 && addr->return_filename)
  {
  BOOL return_output = FALSE;
  struct stat statbuf;
  (void)EXIMfsync(addr->return_file);

  /* If there is no output, do nothing. */

  if (fstat(addr->return_file, &statbuf) == 0 && statbuf.st_size > 0)
    {
    transport_instance *tb = addr->transport;

    /* Handle logging options */

    if (  tb->log_output
       || result == FAIL  && tb->log_fail_output
       || result == DEFER && tb->log_defer_output
       )
      {
      uschar *s;
      FILE *f = Ufopen(addr->return_filename, "rb");
      if (!f)
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to open %s to log output "
          "from %s transport: %s", addr->return_filename, tb->name,
          strerror(errno));
      else
        if ((s = US Ufgets(big_buffer, big_buffer_size, f)))
          {
          uschar *p = big_buffer + Ustrlen(big_buffer);
	  const uschar * sp;
          while (p > big_buffer && isspace(p[-1])) p--;
          *p = 0;
          sp = string_printing(big_buffer);
          log_write(0, LOG_MAIN, "<%s>: %s transport output: %s",
            addr->address, tb->name, sp);
          }
        (void)fclose(f);
      }

    /* Handle returning options, but only if there is an address to return
    the text to. */

    if (sender_address[0] != 0 || addr->prop.errors_address)
      if (tb->return_output)
        {
        addr->transport_return = result = FAIL;
        if (addr->basic_errno == 0 && !addr->message)
          addr->message = US"return message generated";
        return_output = TRUE;
        }
      else
        if (tb->return_fail_output && result == FAIL) return_output = TRUE;
    }

  /* Get rid of the file unless it might be returned, but close it in
  all cases. */

  if (!return_output)
    {
    Uunlink(addr->return_filename);
    addr->return_filename = NULL;
    addr->return_file = -1;
    }

  (void)close(addr->return_file);
  }

/* The success case happens only after delivery by a transport. */

if (result == OK)
  {
  addr->next = addr_succeed;
  addr_succeed = addr;

  /* Call address_done() to ensure that we don't deliver to this address again,
  and write appropriate things to the message log. If it is a child address, we
  call child_done() to scan the ancestors and mark them complete if this is the
  last child to complete. */

  address_done(addr, now);
  DEBUG(D_deliver) debug_printf("%s delivered\n", addr->address);

  if (!addr->parent)
    deliver_msglog("%s %s: %s%s succeeded\n", now, addr->address,
      driver_name, driver_kind);
  else
    {
    deliver_msglog("%s %s <%s>: %s%s succeeded\n", now, addr->address,
      addr->parent->address, driver_name, driver_kind);
    child_done(addr, now);
    }

  /* Certificates for logging (via events) */
#ifdef SUPPORT_TLS
  tls_out.ourcert = addr->ourcert;
  addr->ourcert = NULL;
  tls_out.peercert = addr->peercert;
  addr->peercert = NULL;

  tls_out.cipher = addr->cipher;
  tls_out.peerdn = addr->peerdn;
  tls_out.ocsp = addr->ocsp;
# ifdef SUPPORT_DANE
  tls_out.dane_verified = testflag(addr, af_dane_verified);
# endif
#endif

  delivery_log(LOG_MAIN, addr, logchar, NULL);

#ifdef SUPPORT_TLS
  tls_free_cert(&tls_out.ourcert);
  tls_free_cert(&tls_out.peercert);
  tls_out.cipher = NULL;
  tls_out.peerdn = NULL;
  tls_out.ocsp = OCSP_NOT_REQ;
# ifdef SUPPORT_DANE
  tls_out.dane_verified = FALSE;
# endif
#endif
  }


/* Soft failure, or local delivery process failed; freezing may be
requested. */

else if (result == DEFER || result == PANIC)
  {
  if (result == PANIC) logflags |= LOG_PANIC;

  /* This puts them on the chain in reverse order. Do not change this, because
  the code for handling retries assumes that the one with the retry
  information is last. */

  addr->next = addr_defer;
  addr_defer = addr;

  /* The only currently implemented special action is to freeze the
  message. Logging of this is done later, just before the -H file is
  updated. */

  if (addr->special_action == SPECIAL_FREEZE)
    {
    deliver_freeze = TRUE;
    deliver_frozen_at = time(NULL);
    update_spool = TRUE;
    }

  /* If doing a 2-stage queue run, we skip writing to either the message
  log or the main log for SMTP defers. */

  if (!queue_2stage || addr->basic_errno != 0)
    deferral_log(addr, now, logflags, driver_name, driver_kind);
  }


/* Hard failure. If there is an address to which an error message can be sent,
put this address on the failed list. If not, put it on the deferred list and
freeze the mail message for human attention. The latter action can also be
explicitly requested by a router or transport. */

else
  {
  /* If this is a delivery error, or a message for which no replies are
  wanted, and the message's age is greater than ignore_bounce_errors_after,
  force the af_ignore_error flag. This will cause the address to be discarded
  later (with a log entry). */

  if (!*sender_address && message_age >= ignore_bounce_errors_after)
    addr->prop.ignore_error = TRUE;

  /* Freeze the message if requested, or if this is a bounce message (or other
  message with null sender) and this address does not have its own errors
  address. However, don't freeze if errors are being ignored. The actual code
  to ignore occurs later, instead of sending a message. Logging of freezing
  occurs later, just before writing the -H file. */

  if (  !addr->prop.ignore_error
     && (  addr->special_action == SPECIAL_FREEZE
        || (sender_address[0] == 0 && !addr->prop.errors_address)
     )  )
    {
    frozen_info = addr->special_action == SPECIAL_FREEZE
      ? US""
      : sender_local && !local_error_message
      ? US" (message created with -f <>)"
      : US" (delivery error message)";
    deliver_freeze = TRUE;
    deliver_frozen_at = time(NULL);
    update_spool = TRUE;

    /* The address is put on the defer rather than the failed queue, because
    the message is being retained. */

    addr->next = addr_defer;
    addr_defer = addr;
    }

  /* Don't put the address on the nonrecipients tree yet; wait until an
  error message has been successfully sent. */

  else
    {
    addr->next = addr_failed;
    addr_failed = addr;
    }

  failure_log(addr, driver_name ? NULL : driver_kind, now);
  }

/* Ensure logging is turned on again in all cases */

disable_logging = FALSE;
}