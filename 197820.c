compileBrailleIndicator (FileInfo * nested, char *ermsg,
			 TranslationTableOpcode opcode,
			 TranslationTableOffset * rule)
{
  CharsString token;
  CharsString cells;
  if (getToken (nested, &token, ermsg))
    if (parseDots (nested, &cells, &token))
      if (!addRule (nested, opcode, NULL, &cells, 0, 0))
	return 0;
  *rule = newRuleOffset;
  return 1;
}