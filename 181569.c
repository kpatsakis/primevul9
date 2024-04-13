extend_fses (guestfs_h *g)
{
  size_t n = g->nr_fses + 1;
  struct inspect_fs *p;

  p = realloc (g->fses, n * sizeof (struct inspect_fs));
  if (p == NULL) {
    perrorf (g, "realloc");
    return -1;
  }

  g->fses = p;
  g->nr_fses = n;

  memset (&g->fses[n-1], 0, sizeof (struct inspect_fs));

  return 0;
}