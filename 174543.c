deliver_get_sender_address (uschar * id)
{
int rc;
uschar * new_sender_address,
       * save_sender_address;
BOOL save_qr = queue_running;
uschar * spoolname;

/* make spool_open_datafile non-noisy on fail */

queue_running = TRUE;

/* Side effect: message_subdir is set for the (possibly split) spool directory */

deliver_datafile = spool_open_datafile(id);
queue_running = save_qr;
if (deliver_datafile < 0)
  return NULL;

/* Save and restore the global sender_address.  I'm not sure if we should
not save/restore all the other global variables too, because
spool_read_header() may change all of them. But OTOH, when this
deliver_get_sender_address() gets called, the current message is done
already and nobody needs the globals anymore. (HS12, 2015-08-21) */

spoolname = string_sprintf("%s-H", id);
save_sender_address = sender_address;

rc = spool_read_header(spoolname, TRUE, TRUE);

new_sender_address = sender_address;
sender_address = save_sender_address;

if (rc != spool_read_OK)
  return NULL;

assert(new_sender_address);

(void)close(deliver_datafile);
deliver_datafile = -1;

return new_sender_address;
}