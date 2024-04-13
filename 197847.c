passGetEmphasis ()
{
  int more = 1;
  passLinepos++;
  passEmphasis = 0;
  while (more)
    {
      switch (passLine.chars[passLinepos])
	{
	case 'i':
	  passEmphasis |= italic;
	  break;
	case 'b':
	  passEmphasis |= bold;
	  break;
	case 'u':
	  passEmphasis |= underline;
	  break;
	case 'c':
	  passEmphasis |= computer_braille;
	  break;
	default:
	  more = 0;
	  break;
	}
      if (more)
	passLinepos++;
    }
  if (!passEmphasis)
    {
      compileError (passNested, "emphasis indicators expected");
      passLinepos--;
      return 0;
    }
  return 1;
}