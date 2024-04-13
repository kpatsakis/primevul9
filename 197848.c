passGetDots ()
{
  CharsString collectDots;
  collectDots.length = 0;
  while (passLinepos < passLine.length && (passLine.chars[passLinepos]
					   == '-'
					   || (passLine.chars[passLinepos] >=
					       '0'
					       && passLine.
					       chars[passLinepos] <= '9')
					   ||
					   ((passLine.
					     chars[passLinepos] | 32) >= 'a'
					    && (passLine.
						chars[passLinepos] | 32) <=
					    'f')))
    collectDots.chars[collectDots.length++] = passLine.chars[passLinepos++];
  if (!parseDots (passNested, &passHoldString, &collectDots))
    return 0;
  return 1;
}