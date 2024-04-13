passGetName ()
{
  TranslationTableCharacterAttributes attr;
  passHoldString.length = 0;
  do
    {
      attr = definedCharOrDots (passNested, passLine.chars[passLinepos],
				0)->attributes;
      if (passHoldString.length == 0)
	{
	  if (!(attr & CTC_Letter))
	    {
	      passLinepos++;
	      continue;
	    }
	}
      if (!(attr & CTC_Letter))
	break;
      passHoldString.chars[passHoldString.length++] =
	passLine.chars[passLinepos];
      passLinepos++;
    }
  while (passLinepos < passLine.length);
  return 1;
}