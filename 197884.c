addPassRule (FileInfo * nested)
{
  TranslationTableOffset *offsetPtr;
  switch (newRule->opcode)
    {
    case CTO_Correct:
      offsetPtr = &table->attribOrSwapRules[0];
      break;
    case CTO_Context:
      offsetPtr = &table->attribOrSwapRules[1];
      break;
    case CTO_Pass2:
      offsetPtr = &table->attribOrSwapRules[2];
      break;
    case CTO_Pass3:
      offsetPtr = &table->attribOrSwapRules[3];
      break;
    case CTO_Pass4:
      offsetPtr = &table->attribOrSwapRules[4];
      break;
    default:
      return 0;
    }
  makeRuleChain (offsetPtr);
  return 1;
}