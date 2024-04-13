queue_list(int option, uschar **list, int count)
{
int i;
int subcount;
int now = (int)time(NULL);
void *reset_point;
queue_filename * qf = NULL;
uschar subdirs[64];

/* If given a list of messages, build a chain containing their ids. */

if (count > 0)
  {
  queue_filename *last = NULL;
  for (i = 0; i < count; i++)
    {
    queue_filename *next =
      store_get(sizeof(queue_filename) + Ustrlen(list[i]) + 2);
    sprintf(CS next->text, "%s-H", list[i]);
    next->dir_uschar = '*';
    next->next = NULL;
    if (i == 0) qf = next; else last->next = next;
    last = next;
    }
  }

/* Otherwise get a list of the entire queue, in order if necessary. */

else
  qf = queue_get_spool_list(
          -1,             /* entire queue */
          subdirs,        /* for holding sub list */
          &subcount,      /* for subcount */
          option >= 8);   /* randomize if required */

if (option >= 8) option -= 8;

/* Now scan the chain and print information, resetting store used
each time. */

for (reset_point = store_get(0);
    qf;
    spool_clear_header_globals(), store_reset(reset_point), qf = qf->next
    )
  {
  int rc, save_errno;
  int size = 0;
  BOOL env_read;

  message_size = 0;
  message_subdir[0] = qf->dir_uschar;
  rc = spool_read_header(qf->text, FALSE, count <= 0);
  if (rc == spool_read_notopen && errno == ENOENT && count <= 0)
    continue;
  save_errno = errno;

  env_read = (rc == spool_read_OK || rc == spool_read_hdrerror);

  if (env_read)
    {
    int ptr;
    FILE *jread;
    struct stat statbuf;
    uschar * fname = spool_fname(US"input", message_subdir, qf->text, US"");

    ptr = Ustrlen(fname)-1;
    fname[ptr] = 'D';

    /* Add the data size to the header size; don't count the file name
    at the start of the data file, but add one for the notional blank line
    that precedes the data. */

    if (Ustat(fname, &statbuf) == 0)
      size = message_size + statbuf.st_size - SPOOL_DATA_START_OFFSET + 1;
    i = (now - received_time.tv_sec)/60;  /* minutes on queue */
    if (i > 90)
      {
      i = (i + 30)/60;
      if (i > 72) printf("%2dd ", (i + 12)/24); else printf("%2dh ", i);
      }
    else printf("%2dm ", i);

    /* Collect delivered addresses from any J file */

    fname[ptr] = 'J';
    if ((jread = Ufopen(fname, "rb")))
      {
      while (Ufgets(big_buffer, big_buffer_size, jread) != NULL)
        {
        int n = Ustrlen(big_buffer);
        big_buffer[n-1] = 0;
        tree_add_nonrecipient(big_buffer);
        }
      (void)fclose(jread);
      }
    }

  fprintf(stdout, "%s ", string_format_size(size, big_buffer));
  for (i = 0; i < 16; i++) fputc(qf->text[i], stdout);

  if (env_read && sender_address)
    {
    printf(" <%s>", sender_address);
    if (f.sender_set_untrusted) printf(" (%s)", originator_login);
    }

  if (rc != spool_read_OK)
    {
    printf("\n    ");
    if (save_errno == ERRNO_SPOOLFORMAT)
      {
      struct stat statbuf;
      uschar * fname = spool_fname(US"input", message_subdir, qf->text, US"");

      if (Ustat(fname, &statbuf) == 0)
        printf("*** spool format error: size=" OFF_T_FMT " ***",
          statbuf.st_size);
      else printf("*** spool format error ***");
      }
    else printf("*** spool read error: %s ***", strerror(save_errno));
    if (rc != spool_read_hdrerror)
      {
      printf("\n\n");
      continue;
      }
    }

  if (f.deliver_freeze) printf(" *** frozen ***");

  printf("\n");

  if (recipients_list)
    {
    for (i = 0; i < recipients_count; i++)
      {
      tree_node *delivered =
        tree_search(tree_nonrecipients, recipients_list[i].address);
      if (!delivered || option != 1)
        printf("        %s %s\n",
	  delivered ? "D" : " ", recipients_list[i].address);
      if (delivered) delivered->data.val = TRUE;
      }
    if (option == 2 && tree_nonrecipients)
      queue_list_extras(tree_nonrecipients);
    printf("\n");
    }
  }
}