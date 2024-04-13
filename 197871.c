makeDoubleRule (TranslationTableOpcode opcode, TranslationTableOffset
		* singleRule, TranslationTableOffset * doubleRule)
{
  CharsString dots;
  TranslationTableRule *rule;
  if (!*singleRule || *doubleRule)
    return 1;
  rule = (TranslationTableRule *) & table->ruleArea[*singleRule];
  memcpy (dots.chars, &rule->charsdots[0], rule->dotslen * CHARSIZE);
  memcpy (&dots.chars[rule->dotslen], &rule->charsdots[0],
	  rule->dotslen * CHARSIZE);
  dots.length = 2 * rule->dotslen;
  if (!addRule (NULL, opcode, NULL, &dots, 0, 0))
    return 0;
  *doubleRule = newRuleOffset;
  return 1;
}