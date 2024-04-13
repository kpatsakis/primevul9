compileRule (FileInfo * nested)
{
  int ok = 1;
  CharsString token;
  TranslationTableOpcode opcode;
  CharsString ruleChars;
  CharsString ruleDots;
  CharsString cells;
  CharsString scratchPad;
  TranslationTableCharacterAttributes after = 0;
  TranslationTableCharacterAttributes before = 0;
  int k;

  noback = nofor = 0;
doOpcode:
  if (!getToken (nested, &token, NULL))
    return 1;			/*blank line */
  if (token.chars[0] == '#' || token.chars[0] == '<')
    return 1;			/*comment */
  if (nested->lineNumber == 1 && (eqasc2uni ((unsigned char *) "ISO",
					     token.chars, 3) ||
				  eqasc2uni ((unsigned char *) "UTF-8",
					     token.chars, 5)))
    {
      compileHyphenation (nested, &token);
      return 1;
    }
  opcode = getOpcode (nested, &token);
  switch (opcode)
    {				/*Carry out operations */
    case CTO_None:
      break;
    case CTO_IncludeFile:
      {
	CharsString includedFile;
	if (getToken (nested, &token, "include file name"))
	  if (parseChars (nested, &includedFile, &token))
	    if (!includeFile (nested, &includedFile))
	      ok = 0;
	break;
      }
    case CTO_Locale:
      break;
    case CTO_Undefined:
      ok =
	compileBrailleIndicator (nested, "undefined character opcode",
				 CTO_Undefined, &table->undefined);
      break;
    case CTO_CapitalSign:
      ok =
	compileBrailleIndicator (nested, "capital sign", CTO_CapitalRule,
				 &table->capitalSign);
      break;
    case CTO_BeginCapitalSign:
      ok =
	compileBrailleIndicator (nested, "begin capital sign",
				 CTO_BeginCapitalRule,
				 &table->beginCapitalSign);
      break;
    case CTO_LenBegcaps:
      ok = table->lenBeginCaps = compileNumber (nested);
      break;
    case CTO_EndCapitalSign:
      ok =
	compileBrailleIndicator (nested, "end capitals sign",
				 CTO_EndCapitalRule, &table->endCapitalSign);
      break;
    case CTO_FirstWordCaps:
      ok =
	compileBrailleIndicator (nested, "first word capital sign",
				 CTO_FirstWordCapsRule,
				 &table->firstWordCaps);
      break;
    case CTO_LastWordCapsBefore:
      ok =
	compileBrailleIndicator (nested, "capital sign before last word",
				 CTO_LastWordCapsBeforeRule,
				 &table->lastWordCapsBefore);
      break;
    case CTO_LastWordCapsAfter:
      ok =
	compileBrailleIndicator (nested, "capital sign after last word",
				 CTO_LastWordCapsAfterRule,
				 &table->lastWordCapsAfter);
      break;
    case CTO_LenCapsPhrase:
      ok = table->lenCapsPhrase = compileNumber (nested);
      break;
    case CTO_LetterSign:
      ok =
	compileBrailleIndicator (nested, "letter sign", CTO_LetterRule,
				 &table->letterSign);
      break;
    case CTO_NoLetsignBefore:
      if (getRuleCharsText (nested, &ruleChars))
	{
	  if ((table->noLetsignBeforeCount + ruleChars.length) > LETSIGNSIZE)
	    {
	      compileError (nested, "More than %d characters", LETSIGNSIZE);
	      ok = 0;
	      break;
	    }
	  for (k = 0; k < ruleChars.length; k++)
	    table->noLetsignBefore[table->noLetsignBeforeCount++] =
	      ruleChars.chars[k];
	}
      break;
    case CTO_NoLetsign:
      if (getRuleCharsText (nested, &ruleChars))
	{
	  if ((table->noLetsignCount + ruleChars.length) > LETSIGNSIZE)
	    {
	      compileError (nested, "More than %d characters", LETSIGNSIZE);
	      ok = 0;
	      break;
	    }
	  for (k = 0; k < ruleChars.length; k++)
	    table->noLetsign[table->noLetsignCount++] = ruleChars.chars[k];
	}
      break;
    case CTO_NoLetsignAfter:
      if (getRuleCharsText (nested, &ruleChars))
	{
	  if ((table->noLetsignAfterCount + ruleChars.length) > LETSIGNSIZE)
	    {
	      compileError (nested, "More than %d characters", LETSIGNSIZE);
	      ok = 0;
	      break;
	    }
	  for (k = 0; k < ruleChars.length; k++)
	    table->noLetsignAfter[table->noLetsignAfterCount++] =
	      ruleChars.chars[k];
	}
      break;
    case CTO_NumberSign:
      ok =
	compileBrailleIndicator (nested, "number sign", CTO_NumberRule,
				 &table->numberSign);
      break;
    case CTO_FirstWordItal:
      ok =
	compileBrailleIndicator (nested, "first word italic",
				 CTO_FirstWordItalRule,
				 &table->firstWordItal);
      break;
    case CTO_ItalSign:
    case CTO_LastWordItalBefore:
      ok =
	compileBrailleIndicator (nested, "first word italic before",
				 CTO_LastWordItalBeforeRule,
				 &table->lastWordItalBefore);
      break;
    case CTO_LastWordItalAfter:
      ok =
	compileBrailleIndicator (nested, "last word italic after",
				 CTO_LastWordItalAfterRule,
				 &table->lastWordItalAfter);
      break;
    case CTO_BegItal:
    case CTO_FirstLetterItal:
      ok =
	compileBrailleIndicator (nested, "first letter italic",
				 CTO_FirstLetterItalRule,
				 &table->firstLetterItal);
      break;
    case CTO_EndItal:
    case CTO_LastLetterItal:
      ok =
	compileBrailleIndicator (nested, "last letter italic",
				 CTO_LastLetterItalRule,
				 &table->lastLetterItal);
      break;
    case CTO_SingleLetterItal:
      ok =
	compileBrailleIndicator (nested, "single letter italic",
				 CTO_SingleLetterItalRule,
				 &table->singleLetterItal);
      break;
    case CTO_ItalWord:
      ok =
	compileBrailleIndicator (nested, "italic word", CTO_ItalWordRule,
				 &table->italWord);
      break;
    case CTO_LenItalPhrase:
      ok = table->lenItalPhrase = compileNumber (nested);
      break;
    case CTO_FirstWordBold:
      ok =
	compileBrailleIndicator (nested, "first word bold",
				 CTO_FirstWordBoldRule,
				 &table->firstWordBold);
      break;
    case CTO_BoldSign:
    case CTO_LastWordBoldBefore:
      ok =
	compileBrailleIndicator (nested, "last word bold before",
				 CTO_LastWordBoldBeforeRule,
				 &table->lastWordBoldBefore);
      break;
    case CTO_LastWordBoldAfter:
      ok =
	compileBrailleIndicator (nested, "last word bold after",
				 CTO_LastWordBoldAfterRule,
				 &table->lastWordBoldAfter);
      break;
    case CTO_BegBold:
    case CTO_FirstLetterBold:
      ok =
	compileBrailleIndicator (nested, "first  letter bold",
				 CTO_FirstLetterBoldRule,
				 &table->firstLetterBold);
      break;
    case CTO_EndBold:
    case CTO_LastLetterBold:
      ok =
	compileBrailleIndicator (nested, "last letter bold",
				 CTO_LastLetterBoldRule,
				 &table->lastLetterBold);
      break;
    case CTO_SingleLetterBold:
      ok =
	compileBrailleIndicator (nested, "single  letter bold",
				 CTO_SingleLetterBoldRule,
				 &table->singleLetterBold);
      break;
    case CTO_BoldWord:
      ok =
	compileBrailleIndicator (nested, "bold word", CTO_BoldWordRule,
				 &table->boldWord);
      break;
    case CTO_LenBoldPhrase:
      ok = table->lenBoldPhrase = compileNumber (nested);
      break;
    case CTO_FirstWordUnder:
      ok =
	compileBrailleIndicator (nested, "first word  underline",
				 CTO_FirstWordUnderRule,
				 &table->firstWordUnder);
      break;
    case CTO_UnderSign:
    case CTO_LastWordUnderBefore:
      ok =
	compileBrailleIndicator (nested, "last word underline before",
				 CTO_LastWordUnderBeforeRule,
				 &table->lastWordUnderBefore);
      break;
    case CTO_LastWordUnderAfter:
      ok =
	compileBrailleIndicator (nested, "last  word underline after",
				 CTO_LastWordUnderAfterRule,
				 &table->lastWordUnderAfter);
      break;
    case CTO_BegUnder:
    case CTO_FirstLetterUnder:
      ok =
	compileBrailleIndicator (nested, "first letter underline",
				 CTO_FirstLetterUnderRule,
				 &table->firstLetterUnder);
      break;
    case CTO_EndUnder:
    case CTO_LastLetterUnder:
      ok =
	compileBrailleIndicator (nested, "last letter underline",
				 CTO_LastLetterUnderRule,
				 &table->lastLetterUnder);
      break;
    case CTO_SingleLetterUnder:
      ok =
	compileBrailleIndicator (nested, "single letter underline",
				 CTO_SingleLetterUnderRule,
				 &table->singleLetterUnder);
      break;
    case CTO_UnderWord:
      ok =
	compileBrailleIndicator (nested, "underlined word", CTO_UnderWordRule,
				 &table->underWord);
      break;
    case CTO_LenUnderPhrase:
      ok = table->lenUnderPhrase = compileNumber (nested);
      break;
    case CTO_BegComp:
      ok =
	compileBrailleIndicator (nested, "begin computer braille",
				 CTO_BegCompRule, &table->begComp);
      break;
    case CTO_EndComp:
      ok =
	compileBrailleIndicator (nested, "end computer braslle",
				 CTO_EndCompRule, &table->endComp);
      break;
    case CTO_Syllable:
      table->syllables = 1;
    case CTO_Always:
    case CTO_NoCross:
    case CTO_LargeSign:
    case CTO_WholeWord:
    case CTO_PartWord:
    case CTO_JoinNum:
    case CTO_JoinableWord:
    case CTO_LowWord:
    case CTO_SuffixableWord:
    case CTO_PrefixableWord:
    case CTO_BegWord:
    case CTO_BegMidWord:
    case CTO_MidWord:
    case CTO_MidEndWord:
    case CTO_EndWord:
    case CTO_PrePunc:
    case CTO_PostPunc:
    case CTO_BegNum:
    case CTO_MidNum:
    case CTO_EndNum:
    case CTO_Repeated:
    case CTO_RepWord:
      if (getRuleCharsText (nested, &ruleChars))
	if (getRuleDotsPattern (nested, &ruleDots))
	  if (!addRule (nested, opcode, &ruleChars, &ruleDots, after, before))
	    ok = 0;
      break;
    case CTO_CompDots:
    case CTO_Comp6:
      if (!getRuleCharsText (nested, &ruleChars))
	return 0;
      if (ruleChars.length != 1 || ruleChars.chars[0] > 255)
	{
	  compileError (nested,
			"first operand must be 1 character and < 256");
	  return 0;
	}
      if (!getRuleDotsPattern (nested, &ruleDots))
	return 0;
      if (!addRule (nested, opcode, &ruleChars, &ruleDots, after, before))
	ok = 0;
      table->compdotsPattern[ruleChars.chars[0]] = newRuleOffset;
      break;
    case CTO_ExactDots:
      if (!getRuleCharsText (nested, &ruleChars))
	return 0;
      if (ruleChars.chars[0] != '@')
	{
	  compileError (nested, "The operand must begin with an at sign (@)");
	  return 0;
	}
      for (k = 1; k < ruleChars.length; k++)
	scratchPad.chars[k - 1] = ruleChars.chars[k];
      scratchPad.length = ruleChars.length - 1;
      if (!parseDots (nested, &ruleDots, &scratchPad))
	return 0;
      if (!addRule (nested, opcode, &ruleChars, &ruleDots, before, after))
	ok = 0;
      break;
    case CTO_CapsNoCont:
      ruleChars.length = 1;
      ruleChars.chars[0] = 'a';
      if (!addRule
	  (nested, CTO_CapsNoContRule, &ruleChars, NULL, after, before))
	ok = 0;
      table->capsNoCont = newRuleOffset;
      break;
    case CTO_Replace:
      if (getRuleCharsText (nested, &ruleChars))
	{
	  if (lastToken)
	    ruleDots.length = ruleDots.chars[0] = 0;
	  else
	    {
	      getRuleDotsText (nested, &ruleDots);
	      if (ruleDots.chars[0] == '#')
		ruleDots.length = ruleDots.chars[0] = 0;
	      else if (ruleDots.chars[0] == '\\' && ruleDots.chars[1] == '#')
		memcpy (&ruleDots.chars[0], &ruleDots.chars[1],
			ruleDots.length-- * CHARSIZE);
	    }
	}
      for (k = 0; k < ruleChars.length; k++)
	addCharOrDots (nested, ruleChars.chars[k], 0);
      for (k = 0; k < ruleDots.length; k++)
	addCharOrDots (nested, ruleDots.chars[k], 0);
      if (!addRule (nested, opcode, &ruleChars, &ruleDots, after, before))
	ok = 0;
      break;
    case CTO_Pass2:
      if (table->numPasses < 2)
	table->numPasses = 2;
      goto doPass;
    case CTO_Pass3:
      if (table->numPasses < 3)
	table->numPasses = 3;
      goto doPass;
    case CTO_Pass4:
      if (table->numPasses < 4)
	table->numPasses = 4;
    doPass:
    case CTO_Context:
      if (!compilePassOpcode (nested, opcode))
	ok = 0;
      break;
    case CTO_Correct:
      if (!compilePassOpcode (nested, opcode))
	ok = 0;
      table->corrections = 1;
      break;
    case CTO_Contraction:
    case CTO_NoCont:
    case CTO_CompBrl:
    case CTO_Literal:
      if (getRuleCharsText (nested, &ruleChars))
	if (!addRule (nested, opcode, &ruleChars, NULL, after, before))
	  ok = 0;
      break;
    case CTO_MultInd:
      {
	int lastToken;
	ruleChars.length = 0;
	if (getToken (nested, &token, "multiple braille indicators") &&
	    parseDots (nested, &cells, &token))
	  {
	    while ((lastToken = getToken (nested, &token, "multind opcodes")))
	      {
		opcode = getOpcode (nested, &token);
		if (opcode >= CTO_CapitalSign && opcode < CTO_MultInd)
		  ruleChars.chars[ruleChars.length++] = (widechar) opcode;
		else
		  {
		    compileError (nested, "Not a braille indicator opcode.");
		    ok = 0;
		  }
		if (lastToken == 2)
		  break;
	      }
	  }
	else
	  ok = 0;
	if (!addRule (nested, CTO_MultInd, &ruleChars, &cells, after, before))
	  ok = 0;
	break;
      }

    case CTO_Class:
      {
	CharsString characters;
	const struct CharacterClass *class;
	if (!characterClasses)
	  {
	    if (!allocateCharacterClasses ())
	      ok = 0;
	  }
	if (getToken (nested, &token, "character class name"))
	  {
	    if ((class = findCharacterClass (&token)))
	      {
		compileError (nested, "character class already defined.");
	      }
	    else
	      if ((class =
		   addCharacterClass (nested, &token.chars[0], token.length)))
	      {
		if (getCharacters (nested, &characters))
		  {
		    int index;
		    for (index = 0; index < characters.length; ++index)
		      {
			TranslationTableRule *defRule;
			TranslationTableCharacter *character =
			  definedCharOrDots
			  (nested, characters.chars[index], 0);
			character->attributes |= class->attribute;
			defRule = (TranslationTableRule *)
			  & table->ruleArea[character->definitionRule];
			if (defRule->dotslen == 1)
			  {
			    character = definedCharOrDots
			      (nested,
			       defRule->charsdots[defRule->charslen], 1);
			    character->attributes |= class->attribute;
			  }
		      }
		  }
	      }
	  }
	break;
      }

      {
	TranslationTableCharacterAttributes *attributes;
	const struct CharacterClass *class;
    case CTO_After:
	attributes = &after;
	goto doClass;
    case CTO_Before:
	attributes = &before;
      doClass:

	if (!characterClasses)
	  {
	    if (!allocateCharacterClasses ())
	      ok = 0;
	  }
	if (getCharacterClass (nested, &class))
	  {
	    *attributes |= class->attribute;
	    goto doOpcode;
	  }
	break;
      }
    case CTO_NoBack:
      noback = 1;
      goto doOpcode;
    case CTO_NoFor:
      nofor = 1;
      goto doOpcode;
    case CTO_SwapCc:
    case CTO_SwapCd:
    case CTO_SwapDd:
      if (!compileSwap (nested, opcode))
	ok = 0;
      break;
    case CTO_Hyphen:
    case CTO_DecPoint:
      if (getRuleCharsText (nested, &ruleChars))
	if (getRuleDotsPattern (nested, &ruleDots))
	  {
	    if (ruleChars.length != 1 || ruleDots.length < 1)
	      {
		compileError (nested,
			      "One Unicode character and at least one cell are required.");
		ok = 0;
	      }
	    if (!addRule
		(nested, opcode, &ruleChars, &ruleDots, after, before))
	      ok = 0;
	  }
      break;
    case CTO_Space:
      compileCharDef (nested, opcode, CTC_Space);
      break;
    case CTO_Digit:
      compileCharDef (nested, opcode, CTC_Digit);
      break;
    case CTO_LitDigit:
      compileCharDef (nested, opcode, CTC_LitDigit);
      break;
    case CTO_Punctuation:
      compileCharDef (nested, opcode, CTC_Punctuation);
      break;
    case CTO_Math:
      compileCharDef (nested, opcode, CTC_Math);
      break;
    case CTO_Sign:
      compileCharDef (nested, opcode, CTC_Sign);
      break;
    case CTO_Letter:
      compileCharDef (nested, opcode, CTC_Letter);
      break;
    case CTO_UpperCase:
      compileCharDef (nested, opcode, CTC_UpperCase);
      break;
    case CTO_LowerCase:
      compileCharDef (nested, opcode, CTC_LowerCase);
      break;
    case CTO_NoBreak:
      ok = compileNoBreak (nested);
      break;
    case CTO_Grouping:
      ok = compileGrouping (nested);
      break;
    case CTO_UpLow:
      ok = compileUplow (nested);
      break;
    case CTO_Display:
      if (getRuleCharsText (nested, &ruleChars))
	if (getRuleDotsPattern (nested, &ruleDots))
	  {
	    if (ruleChars.length != 1 || ruleDots.length != 1)
	      {
		compileError (nested,
			      "Exactly one character and one cell are required.");
		ok = 0;
	      }
	    putCharAndDots (nested, ruleChars.chars[0], ruleDots.chars[0]);
	  }
      break;
    default:
      compileError (nested, "unimplemented opcode.");
      break;
    }
  return ok;
}