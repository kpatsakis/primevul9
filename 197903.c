findRuleName (const CharsString * name)
{
  const struct RuleName *nameRule = ruleNames;
  while (nameRule)
    {
      if ((name->length == nameRule->length) &&
	  (memcmp (&name->chars[0], nameRule->name, CHARSIZE *
		   name->length) == 0))
	return nameRule->ruleOffset;
      nameRule = nameRule->next;
    }
  return 0;
}