compileSwap (FileInfo * nested, TranslationTableOpcode opcode)
{
  CharsString ruleChars;
  CharsString ruleDots;
  CharsString name;
  CharsString matches;
  CharsString replacements;
  if (!getToken (nested, &name, "name operand"))
    return 0;
  if (!getToken (nested, &matches, "matches operand"))
    return 0;
  if (!getToken (nested, &replacements, "replacements operand"))
    return 0;
  if (opcode == CTO_SwapCc || opcode == CTO_SwapCd)
    {
      if (!parseChars (nested, &ruleChars, &matches))
	return 0;
    }
  else
    {
      if (!compileSwapDots (nested, &matches, &ruleChars))
	return 0;
    }
  if (opcode == CTO_SwapCc)
    {
      if (!parseChars (nested, &ruleDots, &replacements))
	return 0;
    }
  else
    {
      if (!compileSwapDots (nested, &replacements, &ruleDots))
	return 0;
    }
  if (!addRule (nested, opcode, &ruleChars, &ruleDots, 0, 0))
    return 0;
  if (!addRuleName (nested, &name))
    return 0;
  return 1;
}