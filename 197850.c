parseDots (FileInfo * nested, CharsString * cells, const CharsString * token)
{
/*get dot patterns */
  widechar cell = 0;		/*assembly place for dots */
  int cellCount = 0;
  int index;
  int start = 0;

  for (index = 0; index < token->length; index++)
    {
      int started = index != start;
      widechar character = token->chars[index];
      switch (character)
	{			/*or dots to make up Braille cell */
	  {
	    int dot;
	case '1':
	    dot = B1;
	    goto haveDot;
	case '2':
	    dot = B2;
	    goto haveDot;
	case '3':
	    dot = B3;
	    goto haveDot;
	case '4':
	    dot = B4;
	    goto haveDot;
	case '5':
	    dot = B5;
	    goto haveDot;
	case '6':
	    dot = B6;
	    goto haveDot;
	case '7':
	    dot = B7;
	    goto haveDot;
	case '8':
	    dot = B8;
	    goto haveDot;
	case '9':
	    dot = B9;
	    goto haveDot;
	case 'a':
	case 'A':
	    dot = B10;
	    goto haveDot;
	case 'b':
	case 'B':
	    dot = B11;
	    goto haveDot;
	case 'c':
	case 'C':
	    dot = B12;
	    goto haveDot;
	case 'd':
	case 'D':
	    dot = B13;
	    goto haveDot;
	case 'e':
	case 'E':
	    dot = B14;
	    goto haveDot;
	case 'f':
	case 'F':
	    dot = B15;
	  haveDot:
	    if (started && !cell)
	      goto invalid;
	    if (cell & dot)
	      {
		compileError (nested, "dot specified more than once.");
		return 0;
	      }
	    cell |= dot;
	    break;
	  }
	case '0':		/*blank */
	  if (started)
	    goto invalid;
	  break;
	case '-':		/*got all dots for this cell */
	  if (!started)
	    {
	      compileError (nested, "missing cell specification.");
	      return 0;
	    }
	  cells->chars[cellCount++] = cell | B16;
	  cell = 0;
	  start = index + 1;
	  break;
	default:
	invalid:
	  compileError (nested, "invalid dot number %s.", showString
			(&character, 1));
	  return 0;
	}
    }
  if (index == start)
    {
      compileError (nested, "missing cell specification.");
      return 0;
    }
  cells->chars[cellCount++] = cell | B16;	/*last cell */
  cells->length = cellCount;
  return 1;
}