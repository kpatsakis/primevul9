deallocateRuleNames ()
{
  while (ruleNames)
    {
      struct RuleName *nameRule = ruleNames;
      ruleNames = ruleNames->next;
      if (nameRule)
	free (nameRule);
    }
}