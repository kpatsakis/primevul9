showDots (widechar const *dots, int length)
{
/* Translate a sequence of dots to the encoding used in dots operands. 
*/
  int bufPos = 0;
  int dotsPos;
  for (dotsPos = 0; bufPos < sizeof (scratchBuf) && dotsPos < length;
       dotsPos++)
    {
      if ((dots[dotsPos] & B1))
	scratchBuf[bufPos++] = '1';
      if ((dots[dotsPos] & B2))
	scratchBuf[bufPos++] = '2';
      if ((dots[dotsPos] & B3))
	scratchBuf[bufPos++] = '3';
      if ((dots[dotsPos] & B4))
	scratchBuf[bufPos++] = '4';
      if ((dots[dotsPos] & B5))
	scratchBuf[bufPos++] = '5';
      if ((dots[dotsPos] & B6))
	scratchBuf[bufPos++] = '6';
      if ((dots[dotsPos] & B7))
	scratchBuf[bufPos++] = '7';
      if ((dots[dotsPos] & B8))
	scratchBuf[bufPos++] = '8';
      if ((dots[dotsPos] & B9))
	scratchBuf[bufPos++] = '9';
      if ((dots[dotsPos] & B10))
	scratchBuf[bufPos++] = 'A';
      if ((dots[dotsPos] & B11))
	scratchBuf[bufPos++] = 'B';
      if ((dots[dotsPos] & B12))
	scratchBuf[bufPos++] = 'C';
      if ((dots[dotsPos] & B13))
	scratchBuf[bufPos++] = 'D';
      if ((dots[dotsPos] & B14))
	scratchBuf[bufPos++] = 'E';
      if ((dots[dotsPos] & B15))
	scratchBuf[bufPos++] = 'F';
      if ((dots[dotsPos] == B16))
	scratchBuf[bufPos++] = '0';
      if (dotsPos != length - 1)
	scratchBuf[bufPos++] = '-';
    }
  scratchBuf[bufPos] = 0;
  return &scratchBuf[0];
}