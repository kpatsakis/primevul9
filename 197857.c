hyphenHashLookup (HyphenHashTab * hashTab, const CharsString * key)
{
  int i, j;
  HyphenHashEntry *e;
  if (key->length == 0)
    return 0;
  i = hyphenStringHash (key) % HYPHENHASHSIZE;
  for (e = hashTab->entries[i]; e; e = e->next)
    {
      if (key->length != e->key->length)
	continue;
      for (j = 0; j < key->length; j++)
	if (key->chars[j] != e->key->chars[j])
	  break;
      if (j == key->length)
	return e->val;
    }
  return DEFAULTSTATE;
}