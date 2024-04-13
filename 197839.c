compileGrouping (FileInfo * nested)
{
  int k;
  CharsString name;
  CharsString groupChars;
  CharsString groupDots;
  CharsString dotsParsed;
  TranslationTableCharacter *charsDotsPtr;
  widechar endChar;
  widechar endDots;
  if (!getToken (nested, &name, "name operand"))
    return 0;
  if (!getRuleCharsText (nested, &groupChars))
    return 0;
  if (!getToken (nested, &groupDots, "dots operand"))
    return 0;
  for (k = 0; k < groupDots.length && groupDots.chars[k] != ','; k++);
  if (k == groupDots.length)
    {
      compileError (nested,
		    "Dots operand must consist of two cells separated by a comma");
      return 0;
    }
  groupDots.chars[k] = '-';
  if (!parseDots (nested, &dotsParsed, &groupDots))
    return 0;
  if (groupChars.length != 2 || dotsParsed.length != 2)
    {
      compileError (nested,
		    "two Unicode characters and two cells separated by a comma are needed.");
      return 0;
    }
  charsDotsPtr = addCharOrDots (nested, groupChars.chars[0], 0);
  charsDotsPtr->attributes |= CTC_Math;
  charsDotsPtr->uppercase = charsDotsPtr->realchar;
  charsDotsPtr->lowercase = charsDotsPtr->realchar;
  charsDotsPtr = addCharOrDots (nested, groupChars.chars[1], 0);
  charsDotsPtr->attributes |= CTC_Math;
  charsDotsPtr->uppercase = charsDotsPtr->realchar;
  charsDotsPtr->lowercase = charsDotsPtr->realchar;
  charsDotsPtr = addCharOrDots (nested, dotsParsed.chars[0], 1);
  charsDotsPtr->attributes |= CTC_Math;
  charsDotsPtr->uppercase = charsDotsPtr->realchar;
  charsDotsPtr->lowercase = charsDotsPtr->realchar;
  charsDotsPtr = addCharOrDots (nested, dotsParsed.chars[1], 1);
  charsDotsPtr->attributes |= CTC_Math;
  charsDotsPtr->uppercase = charsDotsPtr->realchar;
  charsDotsPtr->lowercase = charsDotsPtr->realchar;
  if (!addRule (nested, CTO_Grouping, &groupChars, &dotsParsed, 0, 0))
    return 0;
  if (!addRuleName (nested, &name))
    return 0;
  putCharAndDots (nested, groupChars.chars[0], dotsParsed.chars[0]);
  putCharAndDots (nested, groupChars.chars[1], dotsParsed.chars[1]);
  endChar = groupChars.chars[1];
  endDots = dotsParsed.chars[1];
  groupChars.length = dotsParsed.length = 1;
  if (!addRule (nested, CTO_Math, &groupChars, &dotsParsed, 0, 0))
    return 0;
  groupChars.chars[0] = endChar;
  dotsParsed.chars[0] = endDots;
  if (!addRule (nested, CTO_Math, &groupChars, &dotsParsed, 0, 0))
    return 0;
  return 1;
}