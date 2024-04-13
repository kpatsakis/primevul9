compileUplow (FileInfo * nested)
{
  int k;
  TranslationTableCharacter *upperChar;
  TranslationTableCharacter *lowerChar;
  TranslationTableCharacter *upperCell = NULL;
  TranslationTableCharacter *lowerCell = NULL;
  CharsString ruleChars;
  CharsString ruleDots;
  CharsString upperDots;
  CharsString lowerDots;
  int haveLowerDots = 0;
  TranslationTableCharacterAttributes attr;
  if (!getRuleCharsText (nested, &ruleChars))
    return 0;
  if (!getToken (nested, &ruleDots, "dots operand"))
    return 0;
  for (k = 0; k < ruleDots.length && ruleDots.chars[k] != ','; k++);
  if (k == ruleDots.length)
    {
      if (!parseDots (nested, &upperDots, &ruleDots))
	return 0;
      lowerDots.length = upperDots.length;
      for (k = 0; k < upperDots.length; k++)
	lowerDots.chars[k] = upperDots.chars[k];
      lowerDots.chars[k] = 0;
    }
  else
    {
      haveLowerDots = ruleDots.length;
      ruleDots.length = k;
      if (!parseDots (nested, &upperDots, &ruleDots))
	return 0;
      ruleDots.length = 0;
      k++;
      for (; k < haveLowerDots; k++)
	ruleDots.chars[ruleDots.length++] = ruleDots.chars[k];
      if (!parseDots (nested, &lowerDots, &ruleDots))
	return 0;
    }
  if (ruleChars.length != 2 || upperDots.length < 1)
    {
      compileError (nested,
		    "Exactly two Unicode characters and at least one cell are required.");
      return 0;
    }
  if (haveLowerDots && lowerDots.length < 1)
    {
      compileError (nested, "at least one cell is required after the comma.");
      return 0;
    }
  upperChar = addCharOrDots (nested, ruleChars.chars[0], 0);
  upperChar->attributes |= CTC_Letter | CTC_UpperCase;
  upperChar->uppercase = ruleChars.chars[0];
  upperChar->lowercase = ruleChars.chars[1];
  lowerChar = addCharOrDots (nested, ruleChars.chars[1], 0);
  lowerChar->attributes |= CTC_Letter | CTC_LowerCase;
  lowerChar->uppercase = ruleChars.chars[0];
  lowerChar->lowercase = ruleChars.chars[1];
  for (k = 0; k < upperDots.length; k++)
    if (!compile_findCharOrDots (upperDots.chars[k], 1))
      {
	attr = CTC_Letter | CTC_UpperCase;
	upperCell = addCharOrDots (nested, upperDots.chars[k], 1);
	if (upperDots.length != 1)
	  attr = CTC_Space;
	upperCell->attributes |= attr;
	upperCell->uppercase = upperCell->realchar;
      }
  if (haveLowerDots)
    {
      for (k = 0; k < lowerDots.length; k++)
	if (!compile_findCharOrDots (lowerDots.chars[k], 1))
	  {
	    attr = CTC_Letter | CTC_LowerCase;
	    lowerCell = addCharOrDots (nested, lowerDots.chars[k], 1);
	    if (lowerDots.length != 1)
	      attr = CTC_Space;
	    lowerCell->attributes |= attr;
	    lowerCell->lowercase = lowerCell->realchar;
	  }
    }
  else if (upperCell != NULL && upperDots.length == 1)
    upperCell->attributes |= CTC_LowerCase;
  if (lowerDots.length == 1)
    putCharAndDots (nested, ruleChars.chars[1], lowerDots.chars[0]);
  if (upperCell != NULL)
    upperCell->lowercase = lowerDots.chars[0];
  if (lowerCell != NULL)
    lowerCell->uppercase = upperDots.chars[0];
  if (upperDots.length == 1)
    putCharAndDots (nested, ruleChars.chars[0], upperDots.chars[0]);
  ruleChars.length = 1;
  ruleChars.chars[2] = ruleChars.chars[0];
  ruleChars.chars[0] = ruleChars.chars[1];
  if (!addRule (nested, CTO_LowerCase, &ruleChars, &lowerDots, 0, 0))
    return 0;
  ruleChars.chars[0] = ruleChars.chars[2];
  if (!addRule (nested, CTO_UpperCase, &ruleChars, &upperDots, 0, 0))
    return 0;
  return 1;
}