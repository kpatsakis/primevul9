hyphenHashFree (HyphenHashTab * hashTab)
{
  int i;
  HyphenHashEntry *e, *next;
  for (i = 0; i < HYPHENHASHSIZE; i++)
    for (e = hashTab->entries[i]; e; e = next)
      {
	next = e->next;
	free (e->key);
	free (e);
      }
  free (hashTab);
}