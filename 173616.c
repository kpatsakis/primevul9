compileBrailleIndicator(FileInfo *nested, char *ermsg, TranslationTableOpcode opcode,
		TranslationTableOffset *rule, int *lastToken,
		TranslationTableOffset *newRuleOffset, TranslationTableRule **newRule, int noback,
		int nofor, TranslationTableHeader **table) {
	CharsString token;
	CharsString cells;
	if (getToken(nested, &token, ermsg, lastToken))
		if (parseDots(nested, &cells, &token))
			if (!addRule(nested, opcode, NULL, &cells, 0, 0, newRuleOffset, newRule,
						noback, nofor, table))
				return 0;
	*rule = *newRuleOffset;
	return 1;
}