queue_count(void)
{
int subcount;
int count = 0;
queue_filename *f = NULL;
uschar subdirs[64];
f = queue_get_spool_list(
        -1,             /* entire queue */
        subdirs,        /* for holding sub list */
        &subcount,      /* for subcount */
        FALSE);         /* not random */
for (; f != NULL; f = f->next) count++;
fprintf(stdout, "%d\n", count);
}