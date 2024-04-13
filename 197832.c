add_1_multiple ()
{
/*direction = 1, newRule->dotslen > 1*/
  TranslationTableRule *currentRule = NULL;
  TranslationTableOffset *currentOffsetPtr = &table->backRules[stringHash
							       (&newRule->
								charsdots
								[newRule->
								 charslen])];
  if (newRule->opcode == CTO_NoBreak || newRule->opcode == CTO_SwapCc ||
      (newRule->opcode >= CTO_Context && newRule->opcode <= CTO_Pass4))
    return;
  while (*currentOffsetPtr)
    {
      int currentLength;
      int newLength;
      currentRule = (TranslationTableRule *)
	& table->ruleArea[*currentOffsetPtr];
      currentLength = currentRule->dotslen + currentRule->charslen;
      newLength = newRule->dotslen + newRule->charslen;
      if (newLength > currentLength)
	break;
      if (currentLength == newLength)
	if ((currentRule->opcode == CTO_Always)
	    && (newRule->opcode != CTO_Always))
	  break;
      currentOffsetPtr = &currentRule->dotsnext;
    }
  newRule->dotsnext = *currentOffsetPtr;
  *currentOffsetPtr = newRuleOffset;
}