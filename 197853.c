compileCharDef (FileInfo * nested,
		TranslationTableOpcode opcode,
		TranslationTableCharacterAttributes attributes)
{
  CharsString ruleChars;
  CharsString ruleDots;
  TranslationTableCharacter *character;
  TranslationTableCharacter *cell;
  TranslationTableCharacter *otherCell;
  TranslationTableCharacterAttributes attr;
  int k;
  if (!getRuleCharsText (nested, &ruleChars))
    return 0;
  if (attributes & (CTC_UpperCase | CTC_LowerCase))
    attributes |= CTC_Letter;
  if (!getRuleDotsPattern (nested, &ruleDots))
    return 0;
  if (ruleChars.length != 1 || ruleDots.length < 1)
    {
      compileError (nested,
		    "Exactly one Unicode character and at least one cell are required.");
      return 0;
    }
  character = addCharOrDots (nested, ruleChars.chars[0], 0);
  character->attributes |= attributes;
  character->uppercase = character->lowercase = character->realchar;
  cell = compile_findCharOrDots (ruleDots.chars[0], 1);
  if (ruleDots.length == 1 && cell)
    cell->attributes |= attributes;
  else
    {
      for (k = 0; k < ruleDots.length; k++)
	{
	  if (!compile_findCharOrDots (ruleDots.chars[k], 1))
	    {
	      attr = attributes;
	      otherCell = addCharOrDots (nested, ruleDots.chars[k], 1);
	      if (ruleDots.length != 1)
		attr = CTC_Space;
	      otherCell->attributes |= attr;
	      otherCell->uppercase = otherCell->lowercase =
		otherCell->realchar;
	    }
	}
    }
  if (!addRule (nested, opcode, &ruleChars, &ruleDots, 0, 0))
    return 0;
  if (ruleDots.length == 1)
    putCharAndDots (nested, ruleChars.chars[0], ruleDots.chars[0]);
  return 1;
}