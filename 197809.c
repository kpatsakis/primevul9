passGetAttributes ()
{
  int more = 1;
  passAttributes = 0;
  while (more)
    {
      switch (passLine.chars[passLinepos])
	{
	case pass_any:
	  passAttributes = 0xffffffff;
	  break;
	case pass_digit:
	  passAttributes |= CTC_Digit;
	  break;
	case pass_litDigit:
	  passAttributes |= CTC_LitDigit;
	  break;
	case pass_letter:
	  passAttributes |= CTC_Letter;
	  break;
	case pass_math:
	  passAttributes |= CTC_Math;
	  break;
	case pass_punctuation:
	  passAttributes |= CTC_Punctuation;
	  break;
	case pass_sign:
	  passAttributes |= CTC_Sign;
	  break;
	case pass_space:
	  passAttributes |= CTC_Space;
	  break;
	case pass_uppercase:
	  passAttributes |= CTC_UpperCase;
	  break;
	case pass_lowercase:
	  passAttributes |= CTC_LowerCase;
	  break;
	case pass_class1:
	  passAttributes |= CTC_Class1;
	  break;
	case pass_class2:
	  passAttributes |= CTC_Class2;
	  break;
	case pass_class3:
	  passAttributes |= CTC_Class3;
	  break;
	case pass_class4:
	  passAttributes |= CTC_Class4;
	  break;
	default:
	  more = 0;
	  break;
	}
      if (more)
	passLinepos++;
    }
  if (!passAttributes)
    {
      compileError (passNested, "Missing attribute");
      passLinepos--;
      return 0;
    }
  return 1;
}