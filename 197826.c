  addRule
  (FileInfo * nested,
   TranslationTableOpcode opcode,
   CharsString * ruleChars,
   CharsString * ruleDots,
   TranslationTableCharacterAttributes after,
   TranslationTableCharacterAttributes before)
{
/*Add a rule to the table, using the hash function to find the start of 
* chains and chaining both the chars and dots strings */
  int ruleSize = sizeof (TranslationTableRule) - (DEFAULTRULESIZE * CHARSIZE);
  int direction = 0;		/*0 = forward translation; 1 = bacward */
  if (ruleChars)
    ruleSize += CHARSIZE * ruleChars->length;
  if (ruleDots)
    ruleSize += CHARSIZE * ruleDots->length;
  if (!allocateSpaceInTable (nested, &newRuleOffset, ruleSize))
    return 0;
  newRule = (TranslationTableRule *) & table->ruleArea[newRuleOffset];
  newRule->opcode = opcode;
  newRule->after = after;
  newRule->before = before;
  if (ruleChars)
    memcpy (&newRule->charsdots[0], &ruleChars->chars[0],
	    CHARSIZE * (newRule->charslen = ruleChars->length));
  else
    newRule->charslen = 0;
  if (ruleDots)
    memcpy (&newRule->charsdots[newRule->charslen],
	    &ruleDots->chars[0], CHARSIZE * (newRule->dotslen =
					     ruleDots->length));
  else
    newRule->dotslen = 0;
  if (!charactersDefined (nested))
    return 0;

  /*link new rule into table. */
  if (opcode == CTO_SwapCc || opcode == CTO_SwapCd || opcode == CTO_SwapDd)
    return 1;
  if (opcode >= CTO_Context && opcode <= CTO_Pass4 && newRule->charslen == 0)
    return addPassRule (nested);
  if (newRule->charslen == 0 || nofor)
    direction = 1;
  while (direction < 2)
    {
      if (direction == 0 && newRule->charslen == 1)
	add_0_single (nested);
      else if (direction == 0 && newRule->charslen > 1)
	add_0_multiple ();
      else if (direction == 1 && newRule->dotslen == 1 && !noback)
	add_1_single (nested);
      else if (direction == 1 && newRule->dotslen > 1 && !noback)
	add_1_multiple ();
      else
	{
	}
      direction++;
      if (newRule->dotslen == 0)
	direction = 2;
    }
  return 1;
}