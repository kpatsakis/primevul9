passFindName (const CharsString * name)
{
  const struct PassName *curname = passNames;
  CharsString augmentedName;
  for (augmentedName.length = 0; augmentedName.length < name->length;
       augmentedName.length++)
    augmentedName.chars[augmentedName.length] =
      name->chars[augmentedName.length];
  augmentedName.chars[augmentedName.length++] = passOpcode;
  while (curname)
    {
      if ((augmentedName.length == curname->length) &&
	  (memcmp
	   (&augmentedName.chars[0], curname->name,
	    CHARSIZE * name->length) == 0))
	return curname->varnum;
      curname = curname->next;
    }
  compileError (passNested, "name not found");
  return 0;
}