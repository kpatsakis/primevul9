hyphenHashInsert (HyphenHashTab * hashTab, const CharsString * key, int val)
{
  int i, j;
  HyphenHashEntry *e;
  i = hyphenStringHash (key) % HYPHENHASHSIZE;
  if (!(e = malloc (sizeof (HyphenHashEntry))))
    outOfMemory ();
  e->next = hashTab->entries[i];
  e->key = malloc ((key->length + 1) * CHARSIZE);
  if (!e->key)
    outOfMemory ();
  e->key->length = key->length;
  for (j = 0; j < key->length; j++)
    e->key->chars[j] = key->chars[j];
  e->val = val;
  hashTab->entries[i] = e;
}