compilePassOpcode (FileInfo * nested, TranslationTableOpcode opcode)
{
/*Compile the operands of a pass opcode */
  TranslationTableCharacterAttributes after = 0;
  TranslationTableCharacterAttributes before = 0;
  widechar passSubOp;
  const struct CharacterClass *class;
  TranslationTableOffset ruleOffset = 0;
  TranslationTableRule *rule = NULL;
  int k;
  int kk = 0;
  pass_Codes passCode;
  int endTest = 0;
  int isScript = 1;
  passInstructions = passRuleDots.chars;
  passIC = 0;			/*Instruction counter */
  passRuleChars.length = 0;
  passNested = nested;
  passOpcode = opcode;
/* passHoldString and passLine are static variables declared 
 * previously.*/
  passLinepos = 0;
  passHoldString.length = 0;
  for (k = nested->linepos; k < nested->linelen; k++)
    passHoldString.chars[passHoldString.length++] = nested->line[k];
  if (!eqasc2uni ((unsigned char *) "script", passHoldString.chars, 6))
    {
      isScript = 0;
#define SEPCHAR 0x0001
      for (k = 0; k < passHoldString.length && passHoldString.chars[k] > 32;
	   k++);
      if (k < passHoldString.length)
	passHoldString.chars[k] = SEPCHAR;
      else
	{
	  compileError (passNested, "Invalid multipass operands");
	  return 0;
	}
    }
  parseChars (passNested, &passLine, &passHoldString);
  if (isScript)
    {
      int more = 1;
      passCode = passGetScriptToken ();
      if (passCode != pass_script)
	{
	  compileError (passNested, "Invalid multipass statement");
	  return 0;
	}
      /* Declaratives */
      while (more)
	{
	  passCode = passGetScriptToken ();
	  switch (passCode)
	    {
	    case pass_define:
	      if (!passIsLeftParen ())
		return 0;
	      if (!passIsName ())
		return 0;
	      if (!passIsComma ())
		return 0;
	      if (!passIsNumber ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      passAddName (&passHoldString, passHoldNumber);
	      break;
	    case pass_if:
	      more = 0;
	      break;
	    default:
	      compileError (passNested,
			    "invalid definition in declarative part");
	      return 0;
	    }
	}
      /* if part */
      more = 1;
      while (more)
	{
	  passCode = passGetScriptToken ();
	  passSubOp = passCode;
	  switch (passCode)
	    {
	    case pass_not:
	      passInstructions[passIC++] = pass_not;
	      break;
	    case pass_first:
	      passInstructions[passIC++] = pass_first;
	      break;
	    case pass_last:
	      passInstructions[passIC++] = pass_last;
	      break;
	    case pass_search:
	      passInstructions[passIC++] = pass_search;
	      break;
	    case pass_string:
	      if (opcode != CTO_Context && opcode != CTO_Correct)
		{
		  compileError (passNested,
				"Character strings can only be used with the context and correct opcodes.");
		  return 0;
		}
	      passInstructions[passIC++] = pass_string;
	      goto ifDoCharsDots;
	    case pass_dots:
	      if (passOpcode == CTO_Correct || passOpcode == CTO_Context)
		{
		  compileError (passNested,
				"dot patterns cannot be specified in the if part\
 of the correct or context opcodes");
		  return 0;
		}
	      passInstructions[passIC++] = pass_dots;
	    ifDoCharsDots:
	      passInstructions[passIC++] = passHoldString.length;
	      for (kk = 0; kk < passHoldString.length; kk++)
		passInstructions[passIC++] = passHoldString.chars[kk];
	      break;
	    case pass_attributes:
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetAttributes ())
		return 0;
	      if (!passInsertAttributes ())
		return 0;
	      break;
	    case pass_emphasis:
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetEmphasis ())
		return 0;
	      /*Right parenthis handled by subfunctiion */
	      break;
	    case pass_lookback:
	      passInstructions[passIC++] = pass_lookback;
	      passCode = passGetScriptToken ();
	      if (passCode != pass_leftParen)
		{
		  passInstructions[passIC++] = 1;
		  passLinepos = passPrevLinepos;
		  break;
		}
	      if (!passIsNumber ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      passInstructions[passIC] = passHoldNumber;
	      break;
	    case pass_group:
	      if (!passIsLeftParen ())
		return 0;
	      break;
	    case pass_mark:
	      passInstructions[passIC++] = pass_startReplace;
	      passInstructions[passIC++] = pass_endReplace;
	      break;
	    case pass_replace:
	      passInstructions[passIC++] = pass_startReplace;
	      if (!passIsLeftParen ())
		return 0;
	      break;
	    case pass_rightParen:
	      passInstructions[passIC++] = pass_endReplace;
	      break;
	    case pass_groupstart:
	    case pass_groupend:
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetName ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule && rule->opcode == CTO_Grouping)
		{
		  passInstructions[passIC++] = passSubOp;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  break;
		}
	      else
		{
		  compileError (passNested, "%s is not a grouping name",
				showString (&passHoldString.chars[0],
					    passHoldString.length));
		  return 0;
		}
	      break;
	    case pass_class:
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetName ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      if (!(class = findCharacterClass (&passHoldString)))
		return 0;
	      passAttributes = class->attribute;
	      passInsertAttributes ();
	      break;
	    case pass_swap:
	      ruleOffset = findRuleName (&passHoldString);
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetName ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule
		  && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd
		      || rule->opcode == CTO_SwapDd))
		{
		  passInstructions[passIC++] = pass_swap;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  if (!passGetRange ())
		    return 0;
		  break;
		}
	      compileError (passNested,
			    "%s is not a swap name.",
			    showString (&passHoldString.chars[0],
					passHoldString.length));
	      return 0;
	    case pass_nameFound:
	      passHoldNumber = passFindName (&passHoldString);
	      passCode = passGetScriptToken ();
	      if (!(passCode == pass_eq || passCode == pass_lt || passCode
		    == pass_gt || passCode == pass_noteq || passCode ==
		    pass_lteq || passCode == pass_gteq))
		{
		  compileError (nested,
				"invalid comparison operator in if part");
		  return 0;
		}
	      passInstructions[passIC++] = passCode;
	      passInstructions[passIC++] = passHoldNumber;
	      if (!passIsNumber ())
		return 0;
	      passInstructions[passIC++] = passHoldNumber;
	      break;
	    case pass_then:
	      passInstructions[passIC++] = pass_endTest;
	      more = 0;
	      break;
	    default:
	      compileError (passNested, "invalid choice in if part");
	      return 0;
	    }
	}

      /* then part */
      more = 1;
      while (more)
	{
	  passCode = passGetScriptToken ();
	  passSubOp = passCode;
	  switch (passCode)
	    {
	    case pass_string:
	      if (opcode != CTO_Correct)
		{
		  compileError (passNested,
				"Character strings can only be used in the then part with the correct opcode.");
		  return 0;
		}
	      passInstructions[passIC++] = pass_string;
	      goto thenDoCharsDots;
	    case pass_dots:
	      if (opcode == CTO_Correct)
		{
		  compileError (passNested,
				"Dot patterns cannot be used with the correct opcode.");
		  return 0;
		}
	      passInstructions[passIC++] = pass_dots;
	    thenDoCharsDots:
	      passInstructions[passIC++] = passHoldString.length;
	      for (kk = 0; kk < passHoldString.length; kk++)
		passInstructions[passIC++] = passHoldString.chars[kk];
	      break;
	    case pass_nameFound:
	      passHoldNumber = passFindName (&passHoldString);
	      passCode = passGetScriptToken ();
	      if (!(passCode == pass_plus || passCode == pass_hyphen
		    || passCode == pass_eq))
		{
		  compileError (nested,
				"Invalid variable operator in then part");
		  return 0;
		}
	      passInstructions[passIC++] = passCode;
	      passInstructions[passIC++] = passHoldNumber;
	      if (!passIsNumber ())
		return 0;
	      passInstructions[passIC++] = passHoldNumber;
	      break;
	    case pass_copy:
	      passInstructions[passIC++] = pass_copy;
	      break;
	    case pass_omit:
	      passInstructions[passIC++] = pass_omit;
	      break;
	    case pass_swap:
	      ruleOffset = findRuleName (&passHoldString);
	      if (!passIsLeftParen ())
		return 0;
	      if (!passGetName ())
		return 0;
	      if (!passIsRightParen ())
		return 0;
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule
		  && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd
		      || rule->opcode == CTO_SwapDd))
		{
		  passInstructions[passIC++] = pass_swap;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  if (!passGetRange ())
		    return 0;
		  break;
		}
	      compileError (passNested,
			    "%s is not a swap name.",
			    showString (&passHoldString.chars[0],
					passHoldString.length));
	      return 0;
	    case pass_noMoreTokens:
	      more = 0;
	      break;
	    default:
	      compileError (passNested, "invalid action in then part");
	      return 0;
	    }
	}
    }
  else
    {
      /* Older machine-language-like "assembler". */

      /*Compile test part */
      for (k = 0; k < passLine.length && passLine.chars[k] != SEPCHAR; k++);
      endTest = k;
      passLine.chars[endTest] = pass_endTest;
      passLinepos = 0;
      while (passLinepos <= endTest)
	{
	  switch ((passSubOp = passLine.chars[passLinepos]))
	    {
	    case pass_lookback:
	      passInstructions[passIC++] = pass_lookback;
	      passLinepos++;
	      passGetNumber ();
	      if (passHoldNumber == 0)
		passHoldNumber = 1;
	      passInstructions[passIC++] = passHoldNumber;
	      break;
	    case pass_not:
	      passInstructions[passIC++] = pass_not;
	      passLinepos++;
	      break;
	    case pass_first:
	      passInstructions[passIC++] = pass_first;
	      passLinepos++;
	      break;
	    case pass_last:
	      passInstructions[passIC++] = pass_last;
	      passLinepos++;
	      break;
	    case pass_search:
	      passInstructions[passIC++] = pass_search;
	      passLinepos++;
	      break;
	    case pass_string:
	      if (opcode != CTO_Context && opcode != CTO_Correct)
		{
		  compileError (passNested,
				"Character strings can only be used with the context and correct opcodes.");
		  return 0;
		}
	      passLinepos++;
	      passInstructions[passIC++] = pass_string;
	      passGetString ();
	      goto testDoCharsDots;
	    case pass_dots:
	      passLinepos++;
	      passInstructions[passIC++] = pass_dots;
	      passGetDots ();
	    testDoCharsDots:
	      if (passHoldString.length == 0)
		return 0;
	      passInstructions[passIC++] = passHoldString.length;
	      for (kk = 0; kk < passHoldString.length; kk++)
		passInstructions[passIC++] = passHoldString.chars[kk];
	      break;
	    case pass_startReplace:
	      passInstructions[passIC++] = pass_startReplace;
	      passLinepos++;
	      break;
	    case pass_endReplace:
	      passInstructions[passIC++] = pass_endReplace;
	      passLinepos++;
	      break;
	    case pass_variable:
	      passLinepos++;
	      passGetNumber ();
	      switch (passLine.chars[passLinepos])
		{
		case pass_eq:
		  passInstructions[passIC++] = pass_eq;
		  goto doComp;
		case pass_lt:
		  if (passLine.chars[passLinepos + 1] == pass_eq)
		    {
		      passLinepos++;
		      passInstructions[passIC++] = pass_lteq;
		    }
		  else
		    passInstructions[passIC++] = pass_lt;
		  goto doComp;
		case pass_gt:
		  if (passLine.chars[passLinepos + 1] == pass_eq)
		    {
		      passLinepos++;
		      passInstructions[passIC++] = pass_gteq;
		    }
		  else
		    passInstructions[passIC++] = pass_gt;
		doComp:
		  passInstructions[passIC++] = passHoldNumber;
		  passLinepos++;
		  passGetNumber ();
		  passInstructions[passIC++] = passHoldNumber;
		  break;
		default:
		  compileError (passNested, "incorrect comparison operator");
		  return 0;
		}
	      break;
	    case pass_attributes:
	      passLinepos++;
	      passGetAttributes ();
	    insertAttributes:
	      passInstructions[passIC++] = pass_attributes;
	      passInstructions[passIC++] = passAttributes >> 16;
	      passInstructions[passIC++] = passAttributes & 0xffff;
	    getRange:
	      if (passLine.chars[passLinepos] == pass_until)
		{
		  passLinepos++;
		  passInstructions[passIC++] = 1;
		  passInstructions[passIC++] = 0xffff;
		  break;
		}
	      passGetNumber ();
	      if (passHoldNumber == 0)
		{
		  passHoldNumber = passInstructions[passIC++] = 1;
		  passInstructions[passIC++] = 1;	/*This is not an error */
		  break;
		}
	      passInstructions[passIC++] = passHoldNumber;
	      if (passLine.chars[passLinepos] != pass_hyphen)
		{
		  passInstructions[passIC++] = passHoldNumber;
		  break;
		}
	      passLinepos++;
	      passGetNumber ();
	      if (passHoldNumber == 0)
		{
		  compileError (passNested, "invalid range");
		  return 0;
		}
	      passInstructions[passIC++] = passHoldNumber;
	      break;
	    case pass_groupstart:
	    case pass_groupend:
	      passLinepos++;
	      passGetName ();
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule && rule->opcode == CTO_Grouping)
		{
		  passInstructions[passIC++] = passSubOp;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  break;
		}
	      else
		{
		  compileError (passNested, "%s is not a grouping name",
				showString (&passHoldString.chars[0],
					    passHoldString.length));
		  return 0;
		}
	      break;
	    case pass_swap:
	      passGetName ();
	      if ((class = findCharacterClass (&passHoldString)))
		{
		  passAttributes = class->attribute;
		  goto insertAttributes;
		}
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule
		  && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd
		      || rule->opcode == CTO_SwapDd))
		{
		  passInstructions[passIC++] = pass_swap;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  goto getRange;
		}
	      compileError (passNested,
			    "%s is neither a class name nor a swap name.",
			    showString (&passHoldString.chars[0],
					passHoldString.length));
	      return 0;
	    case pass_endTest:
	      passInstructions[passIC++] = pass_endTest;
	      passLinepos++;
	      break;
	    default:
	      compileError (passNested,
			    "incorrect operator '%c ' in test part",
			    passLine.chars[passLinepos]);
	      return 0;
	    }

	}			/*Compile action part */

      /* Compile action part */
      while (passLinepos < passLine.length &&
	     passLine.chars[passLinepos] <= 32)
	passLinepos++;
      while (passLinepos < passLine.length &&
	     passLine.chars[passLinepos] > 32)
	{
	  switch ((passSubOp = passLine.chars[passLinepos]))
	    {
	    case pass_string:
	      if (opcode != CTO_Correct)
		{
		  compileError (passNested,
				"Character strings can only be used with the ccorrect opcode.");
		  return 0;
		}
	      passLinepos++;
	      passInstructions[passIC++] = pass_string;
	      passGetString ();
	      goto actionDoCharsDots;
	    case pass_dots:
	      if (opcode == CTO_Correct)
		{
		  compileError (passNested,
				"Dot patterns cannot be used with the correct opcode.");
		  return 0;
		}
	      passLinepos++;
	      passGetDots ();
	      passInstructions[passIC++] = pass_dots;
	    actionDoCharsDots:
	      if (passHoldString.length == 0)
		return 0;
	      passInstructions[passIC++] = passHoldString.length;
	      for (kk = 0; kk < passHoldString.length; kk++)
		passInstructions[passIC++] = passHoldString.chars[kk];
	      break;
	    case pass_variable:
	      passLinepos++;
	      passGetNumber ();
	      switch (passLine.chars[passLinepos])
		{
		case pass_eq:
		  passInstructions[passIC++] = pass_eq;
		  passInstructions[passIC++] = passHoldNumber;
		  passLinepos++;
		  passGetNumber ();
		  passInstructions[passIC++] = passHoldNumber;
		  break;
		case pass_plus:
		case pass_hyphen:
		  passInstructions[passIC++] = passLine.chars[passLinepos];
		  passInstructions[passIC++] = passHoldNumber;
		  break;
		default:
		  compileError (passNested,
				"incorrect variable operator in action part");
		  return 0;
		}
	      break;
	    case pass_copy:
	      passInstructions[passIC++] = pass_copy;
	      passLinepos++;
	      break;
	    case pass_omit:
	      passInstructions[passIC++] = pass_omit;
	      passLinepos++;
	      break;
	    case pass_groupreplace:
	    case pass_groupstart:
	    case pass_groupend:
	      passLinepos++;
	      passGetName ();
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule && rule->opcode == CTO_Grouping)
		{
		  passInstructions[passIC++] = passSubOp;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  break;
		}
	      compileError (passNested, "%s is not a grouping name",
			    showString (&passHoldString.chars[0],
					passHoldString.length));
	      return 0;
	    case pass_swap:
	      passLinepos++;
	      passGetName ();
	      ruleOffset = findRuleName (&passHoldString);
	      if (ruleOffset)
		rule = (TranslationTableRule *) & table->ruleArea[ruleOffset];
	      if (rule
		  && (rule->opcode == CTO_SwapCc || rule->opcode == CTO_SwapCd
		      || rule->opcode == CTO_SwapDd))
		{
		  passInstructions[passIC++] = pass_swap;
		  passInstructions[passIC++] = ruleOffset >> 16;
		  passInstructions[passIC++] = ruleOffset & 0xffff;
		  break;
		}
	      compileError (passNested, "%s is not a swap name.",
			    showString (&passHoldString.chars[0],
					passHoldString.length));
	      return 0;
	      break;
	    default:
	      compileError (passNested, "incorrect operator in action part");
	      return 0;
	    }
	}
    }

  /*Analyze and add rule */
  passRuleDots.length = passIC;
  passIC = 0;
  while (passIC < passRuleDots.length)
    {
      int start = 0;
      switch (passInstructions[passIC])
	{
	case pass_string:
	case pass_dots:
	case pass_attributes:
	case pass_swap:
	  start = 1;
	  break;
	case pass_groupstart:
	case pass_groupend:
	  start = 1;
	  break;
	case pass_eq:
	case pass_lt:
	case pass_gt:
	case pass_lteq:
	case pass_gteq:
	  passIC += 3;
	  break;
	case pass_lookback:
	  passIC += 2;
	  break;
	case pass_not:
	case pass_startReplace:
	case pass_endReplace:
	case pass_first:
	  passIC++;
	  break;
	default:
	  compileError (passNested,
			"Test/if part must contain characters, dots, attributes or class \
swap.");
	  return 0;
	}
      if (start)
	break;
    }

  switch (passInstructions[passIC])
    {
    case pass_string:
    case pass_dots:
      for (k = 0; k < passInstructions[passIC + 1]; k++)
	passRuleChars.chars[k] = passInstructions[passIC + 2 + k];
      passRuleChars.length = k;
      after = before = 0;
      break;
    case pass_attributes:
    case pass_groupstart:
    case pass_groupend:
    case pass_swap:
      after = passRuleDots.length;
      before = 0;
      break;
    default:
      break;
    }
  if (!addRule (passNested, opcode, &passRuleChars, &passRuleDots,
		after, before))
    return 0;
  return 1;
}