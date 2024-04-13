showString (widechar const *chars, int length)
{
/*Translate a string of characters to the encoding used in character 
* operands */
  int charPos;
  int bufPos = 0;
  scratchBuf[bufPos++] = '\'';
  for (charPos = 0; charPos < length; charPos++)
    {
      if (chars[charPos] >= 32 && chars[charPos] < 127)
	scratchBuf[bufPos++] = (char) chars[charPos];
      else
	{
	  char hexbuf[20];
	  int hexLength;
	  char escapeLetter;

	  int leadingZeros;
	  int hexPos;
	  hexLength = sprintf (hexbuf, "%x", chars[charPos]);
	  switch (hexLength)
	    {
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	      escapeLetter = 'x';
	      leadingZeros = 4 - hexLength;
	      break;
	    case 5:
	      escapeLetter = 'y';
	      leadingZeros = 0;
	      break;
	    case 6:
	    case 7:
	    case 8:
	      escapeLetter = 'z';
	      leadingZeros = 8 - hexLength;
	      break;
	    default:
	      escapeLetter = '?';
	      leadingZeros = 0;
	      break;
	    }
	  if ((bufPos + leadingZeros + hexLength + 4) >= sizeof (scratchBuf))
	    break;
	  scratchBuf[bufPos++] = '\\';
	  scratchBuf[bufPos++] = escapeLetter;
	  for (hexPos = 0; hexPos < leadingZeros; hexPos++)
	    scratchBuf[bufPos++] = '0';
	  for (hexPos = 0; hexPos < hexLength; hexPos++)
	    scratchBuf[bufPos++] = hexbuf[hexPos];
	}
    }
  scratchBuf[bufPos++] = '\'';
  scratchBuf[bufPos] = 0;
  return scratchBuf;
}