makeRuleChain (TranslationTableOffset * offsetPtr)
{
  TranslationTableRule *currentRule;
  while (*offsetPtr)
    {
      currentRule = (TranslationTableRule *) & table->ruleArea[*offsetPtr];
      offsetPtr = &currentRule->charsnext;
    }
  newRule->charsnext = *offsetPtr;
  *offsetPtr = newRuleOffset;
}