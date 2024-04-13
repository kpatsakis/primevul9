queue_get_spool_list(int subdiroffset, uschar *subdirs, int *subcount,
  BOOL randomize)
{
int i;
int flags = 0;
int resetflags = -1;
int subptr;
queue_filename *yield = NULL;
queue_filename *last = NULL;
struct dirent *ent;
DIR *dd;
uschar buffer[256];
queue_filename *root[LOG2_MAXNODES];

/* When randomizing, the file names are added to the start or end of the list
according to the bits of the flags variable. Get a collection of bits from the
current time. Use the bottom 16 and just keep re-using them if necessary. When
not randomizing, initialize the sublists for the bottom-up merge sort. */

if (randomize)
  resetflags = time(NULL) & 0xFFFF;
else
   for (i = 0; i < LOG2_MAXNODES; i++)
     root[i] = NULL;

/* If processing the full queue, or just the top-level, start at the base
directory, and initialize the first subdirectory name (as none). Otherwise,
start at the sub-directory offset. */

if (subdiroffset <= 0)
  {
  i = 0;
  subdirs[0] = 0;
  *subcount = 0;
  }
else
  i = subdiroffset;

/* Set up prototype for the directory name. */

spool_pname_buf(buffer, sizeof(buffer));
buffer[sizeof(buffer) - 3] = 0;
subptr = Ustrlen(buffer);
buffer[subptr+2] = 0;               /* terminator for lengthened name */

/* This loop runs at least once, for the main or given directory, and then as
many times as necessary to scan any subdirectories encountered in the main
directory, if they are to be scanned at this time. */

for (; i <= *subcount; i++)
  {
  int count = 0;
  int subdirchar = subdirs[i];      /* 0 for main directory */

  if (subdirchar != 0)
    {
    buffer[subptr] = '/';
    buffer[subptr+1] = subdirchar;
    }

  DEBUG(D_queue_run) debug_printf("looking in %s\n", buffer);
  if (!(dd = opendir(CS buffer)))
    continue;

  /* Now scan the directory. */

  while ((ent = readdir(dd)))
    {
    uschar *name = US ent->d_name;
    int len = Ustrlen(name);

    /* Count entries */

    count++;

    /* If we find a single alphameric sub-directory in the base directory,
    add it to the list for subsequent scans. */

    if (i == 0 && len == 1 && isalnum(*name))
      {
      *subcount = *subcount + 1;
      subdirs[*subcount] = *name;
      continue;
      }

    /* Otherwise, if it is a header spool file, add it to the list */

    if (len == SPOOL_NAME_LENGTH &&
        Ustrcmp(name + SPOOL_NAME_LENGTH - 2, "-H") == 0)
      {
      queue_filename *next =
        store_get(sizeof(queue_filename) + Ustrlen(name));
      Ustrcpy(next->text, name);
      next->dir_uschar = subdirchar;

      /* Handle the creation of a randomized list. The first item becomes both
      the top and bottom of the list. Subsequent items are inserted either at
      the top or the bottom, randomly. This is, I argue, faster than doing a
      sort by allocating a random number to each item, and it also saves having
      to store the number with each item. */

      if (randomize)
        if (!yield)
          {
          next->next = NULL;
          yield = last = next;
          }
        else
          {
          if (flags == 0)
	    flags = resetflags;
          if ((flags & 1) == 0)
            {
            next->next = yield;
            yield = next;
            }
          else
            {
            next->next = NULL;
            last->next = next;
            last = next;
            }
          flags = flags >> 1;
          }

      /* Otherwise do a bottom-up merge sort based on the name. */

      else
        {
        int j;
        next->next = NULL;
        for (j = 0; j < LOG2_MAXNODES; j++)
          if (root[j])
            {
            next = merge_queue_lists(next, root[j]);
            root[j] = j == LOG2_MAXNODES - 1 ? next : NULL;
            }
          else
            {
            root[j] = next;
            break;
            }
        }
      }
    }

  /* Finished with this directory */

  closedir(dd);

  /* If we have just scanned a sub-directory, and it was empty (count == 2
  implies just "." and ".." entries), and Exim is no longer configured to
  use sub-directories, attempt to get rid of it. At the same time, try to
  get rid of any corresponding msglog subdirectory. These are just cosmetic
  tidying actions, so just ignore failures. If we are scanning just a single
  sub-directory, break the loop. */

  if (i != 0)
    {
    if (!split_spool_directory && count <= 2)
      {
      uschar subdir[2];

      rmdir(CS buffer);
      subdir[0] = subdirchar; subdir[1] = 0;
      rmdir(CS spool_dname(US"msglog", subdir));
      }
    if (subdiroffset > 0) break;    /* Single sub-directory */
    }

  /* If we have just scanned the base directory, and subdiroffset is 0,
  we do not want to continue scanning the sub-directories. */

  else if (subdiroffset == 0)
    break;
  }    /* Loop for multiple subdirectories */

/* When using a bottom-up merge sort, do the final merging of the sublists.
Then pass back the final list of file items. */

if (!randomize)
  for (i = 0; i < LOG2_MAXNODES; ++i)
    yield = merge_queue_lists(yield, root[i]);

return yield;
}