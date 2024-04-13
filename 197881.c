add_1_single (FileInfo * nested)
{
/*direction = 1, newRule->dotslen = 1*/
  TranslationTableRule *currentRule;
  TranslationTableOffset *currentOffsetPtr;
  TranslationTableCharacter *dots;
  if (newRule->opcode == CTO_NoBreak || newRule->opcode == CTO_SwapCc ||
      (newRule->opcode >= CTO_Context
       &&
       newRule->opcode <= CTO_Pass4)
      || newRule->opcode == CTO_Repeated || (newRule->opcode == CTO_Always
					     && newRule->charslen == 1))
    return;			/*too ambiguous */
  dots = definedCharOrDots (nested, newRule->charsdots[newRule->charslen], 1);
  if (newRule->opcode >= CTO_Space && newRule->opcode < CTO_UpLow)
    dots->definitionRule = newRuleOffset;
  currentOffsetPtr = &dots->otherRules;
  while (*currentOffsetPtr)
    {
      currentRule = (TranslationTableRule *)
	& table->ruleArea[*currentOffsetPtr];
      if (newRule->charslen > currentRule->charslen ||
	  currentRule->dotslen == 0)
	break;
      if (currentRule->opcode >= CTO_Space && currentRule->opcode < CTO_UpLow)
	if (!(newRule->opcode >= CTO_Space && newRule->opcode < CTO_UpLow))
	  break;
      currentOffsetPtr = &currentRule->dotsnext;
    }
  newRule->dotsnext = *currentOffsetPtr;
  *currentOffsetPtr = newRuleOffset;
}