hyphenHashNew ()
{
  HyphenHashTab *hashTab;
  if (!(hashTab = malloc (sizeof (HyphenHashTab))))
    outOfMemory ();
  memset (hashTab, 0, sizeof (HyphenHashTab));
  return hashTab;
}