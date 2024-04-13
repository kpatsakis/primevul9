compileNoBreak (FileInfo * nested)
{
  int k;
  CharsString ruleDots;
  CharsString otherDots;
  CharsString dotsBefore;
  CharsString dotsAfter;
  int haveDotsAfter = 0;
  if (!getToken (nested, &ruleDots, "dots operand"))
    return 0;
  for (k = 0; k < ruleDots.length && ruleDots.chars[k] != ','; k++);
  if (k == ruleDots.length)
    {
      if (!parseDots (nested, &dotsBefore, &ruleDots))
	return 0;
      dotsAfter.length = dotsBefore.length;
      for (k = 0; k < dotsBefore.length; k++)
	dotsAfter.chars[k] = dotsBefore.chars[k];
      dotsAfter.chars[k] = 0;
    }
  else
    {
      haveDotsAfter = ruleDots.length;
      ruleDots.length = k;
      if (!parseDots (nested, &dotsBefore, &ruleDots))
	return 0;
      otherDots.length = 0;
      k++;
      for (; k < haveDotsAfter; k++)
	otherDots.chars[otherDots.length++] = ruleDots.chars[k];
      if (!parseDots (nested, &dotsAfter, &otherDots))
	return 0;
    }
  for (k = 0; k < dotsBefore.length; k++)
    dotsBefore.chars[k] = getCharFromDots (dotsBefore.chars[k]);
  for (k = 0; k < dotsAfter.length; k++)
    dotsAfter.chars[k] = getCharFromDots (dotsAfter.chars[k]);
  if (!addRule (nested, CTO_NoBreak, &dotsBefore, &dotsAfter, 0, 0))
    return 0;
  table->noBreak = newRuleOffset;
  return 1;
}