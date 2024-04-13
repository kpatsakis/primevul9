charactersDefined (FileInfo * nested)
{
/*Check that all characters are defined by character-definition 
* opcodes*/
  int noErrors = 1;
  int k;
  if ((newRule->opcode >= CTO_Space && newRule->opcode <= CTO_LitDigit)
      || newRule->opcode == CTO_SwapDd
      ||
      newRule->opcode == CTO_Replace || newRule->opcode == CTO_MultInd
      || newRule->opcode == CTO_Repeated ||
      ((newRule->opcode >= CTO_Context && newRule->opcode <=
	CTO_Pass4) && newRule->opcode != CTO_Correct))
    return 1;
  for (k = 0; k < newRule->charslen; k++)
    if (!compile_findCharOrDots (newRule->charsdots[k], 0))
      {
	compileError (nested, "Character %s is not defined", showString
		      (&newRule->charsdots[k], 1));
	noErrors = 0;
      }
  if (!(newRule->opcode == CTO_Correct || newRule->opcode ==
	CTO_NoBreak || newRule->opcode == CTO_SwapCc || newRule->opcode ==
	CTO_SwapCd))
    {
      for (k = newRule->charslen; k < newRule->charslen + newRule->dotslen;
	   k++)
	if (!compile_findCharOrDots (newRule->charsdots[k], 1))
	  {
	    compileError (nested, "Dot pattern %s is not defined.",
			  unknownDots (newRule->charsdots[k]));
	    noErrors = 0;
	  }
    }
  return noErrors;
}