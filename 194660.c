run_mime_acl(uschar *acl, BOOL *smtp_yield_ptr, uschar **smtp_reply_ptr,
  uschar **blackholed_by_ptr)
{
FILE *mbox_file;
uschar rfc822_file_path[2048];
unsigned long mbox_size;
header_line *my_headerlist;
uschar *user_msg, *log_msg;
int mime_part_count_buffer = -1;
uschar * mbox_filename;
int rc = OK;

memset(CS rfc822_file_path,0,2048);

/* check if it is a MIME message */

for (my_headerlist = header_list; my_headerlist; my_headerlist = my_headerlist->next)
  if (  my_headerlist->type != '*'			/* skip deleted headers */
     && strncmpic(my_headerlist->text, US"Content-Type:", 13) == 0
     )
    {
    DEBUG(D_receive) debug_printf("Found Content-Type: header - executing acl_smtp_mime.\n");
    goto DO_MIME_ACL;
    }

DEBUG(D_receive) debug_printf("No Content-Type: header - presumably not a MIME message.\n");
return TRUE;

DO_MIME_ACL:

/* make sure the eml mbox file is spooled up */
if (!(mbox_file = spool_mbox(&mbox_size, NULL, &mbox_filename)))
  {								/* error while spooling */
  log_write(0, LOG_MAIN|LOG_PANIC,
         "acl_smtp_mime: error while creating mbox spool file, message temporarily rejected.");
  Uunlink(spool_name);
  unspool_mbox();
#ifdef EXPERIMENTAL_DCC
  dcc_ok = 0;
#endif
  smtp_respond(US"451", 3, TRUE, US"temporary local problem");
  message_id[0] = 0;            /* Indicate no message accepted */
  *smtp_reply_ptr = US"";       /* Indicate reply already sent */
  return FALSE;                 /* Indicate skip to end of receive function */
  }

mime_is_rfc822 = 0;

MIME_ACL_CHECK:
mime_part_count = -1;
rc = mime_acl_check(acl, mbox_file, NULL, &user_msg, &log_msg);
(void)fclose(mbox_file);

if (Ustrlen(rfc822_file_path) > 0)
  {
  mime_part_count = mime_part_count_buffer;

  if (unlink(CS rfc822_file_path) == -1)
    {
    log_write(0, LOG_PANIC,
         "acl_smtp_mime: can't unlink RFC822 spool file, skipping.");
      goto END_MIME_ACL;
    }
  }

/* check if we must check any message/rfc822 attachments */
if (rc == OK)
  {
  uschar * scandir;
  struct dirent * entry;
  DIR * tempdir;

  scandir = string_copyn(mbox_filename, Ustrrchr(mbox_filename, '/') - mbox_filename);

  tempdir = opendir(CS scandir);
  for (;;)
    {
    if (!(entry = readdir(tempdir)))
      break;
    if (strncmpic(US entry->d_name, US"__rfc822_", 9) == 0)
      {
      (void) string_format(rfc822_file_path, sizeof(rfc822_file_path),
	"%s/%s", scandir, entry->d_name);
      DEBUG(D_receive) debug_printf("RFC822 attachment detected: running MIME ACL for '%s'\n",
	rfc822_file_path);
      break;
      }
    }
  closedir(tempdir);

  if (entry)
    {
    if ((mbox_file = Ufopen(rfc822_file_path, "rb")))
      {
      /* set RFC822 expansion variable */
      mime_is_rfc822 = 1;
      mime_part_count_buffer = mime_part_count;
      goto MIME_ACL_CHECK;
      }
    log_write(0, LOG_PANIC,
       "acl_smtp_mime: can't open RFC822 spool file, skipping.");
    unlink(CS rfc822_file_path);
    }
  }

END_MIME_ACL:
add_acl_headers(ACL_WHERE_MIME, US"MIME");
if (rc == DISCARD)
  {
  recipients_count = 0;
  *blackholed_by_ptr = US"MIME ACL";
  }
else if (rc != OK)
  {
  Uunlink(spool_name);
  unspool_mbox();
#ifdef EXPERIMENTAL_DCC
  dcc_ok = 0;
#endif
  if (  smtp_input
     && smtp_handle_acl_fail(ACL_WHERE_MIME, rc, user_msg, log_msg) != 0)
    {
    *smtp_yield_ptr = FALSE;    /* No more messages after dropped connection */
    *smtp_reply_ptr = US"";     /* Indicate reply already sent */
    }
  message_id[0] = 0;            /* Indicate no message accepted */
  return FALSE;                 /* Cause skip to end of receive function */
  }

return TRUE;
}