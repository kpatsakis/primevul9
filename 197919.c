add_0_multiple ()
{
/*direction = 0 newRule->charslen > 1*/
  TranslationTableRule *currentRule = NULL;
  TranslationTableOffset *currentOffsetPtr =
    &table->forRules[stringHash (&newRule->charsdots[0])];
  while (*currentOffsetPtr)
    {
      currentRule = (TranslationTableRule *)
	& table->ruleArea[*currentOffsetPtr];
      if (newRule->charslen > currentRule->charslen)
	break;
      if (newRule->charslen == currentRule->charslen)
	if ((currentRule->opcode == CTO_Always)
	    && (newRule->opcode != CTO_Always))
	  break;
      currentOffsetPtr = &currentRule->charsnext;
    }
  newRule->charsnext = *currentOffsetPtr;
  *currentOffsetPtr = newRuleOffset;
}