add_0_single (FileInfo * nested)
{
/*direction = 0, newRule->charslen = 1*/
  TranslationTableRule *currentRule;
  TranslationTableOffset *currentOffsetPtr;
  TranslationTableCharacter *character;
  int m = 0;
  if (newRule->opcode == CTO_CompDots || newRule->opcode == CTO_Comp6)
    return;
  if (newRule->opcode >= CTO_Pass2 && newRule->opcode <= CTO_Pass4)
    m = 1;
  character = definedCharOrDots (nested, newRule->charsdots[0], m);
  if (m != 1 && character->attributes & CTC_Letter && (newRule->opcode
						       ==
						       CTO_WholeWord
						       || newRule->opcode ==
						       CTO_LargeSign))
    {
      if (table->noLetsignCount < LETSIGNSIZE)
	table->noLetsign[table->noLetsignCount++] = newRule->charsdots[0];
    }
  if (newRule->opcode >= CTO_Space && newRule->opcode < CTO_UpLow)
    character->definitionRule = newRuleOffset;
  currentOffsetPtr = &character->otherRules;
  while (*currentOffsetPtr)
    {
      currentRule = (TranslationTableRule *)
	& table->ruleArea[*currentOffsetPtr];
      if (currentRule->charslen == 0)
	break;
      if (currentRule->opcode >= CTO_Space && currentRule->opcode < CTO_UpLow)
	if (!(newRule->opcode >= CTO_Space && newRule->opcode < CTO_UpLow))
	  break;
      currentOffsetPtr = &currentRule->charsnext;
    }
  newRule->charsnext = *currentOffsetPtr;
  *currentOffsetPtr = newRuleOffset;
}