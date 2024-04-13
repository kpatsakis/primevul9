showAttributes (TranslationTableCharacterAttributes a)
{
/* Show attributes using the letters used after the $ in multipass 
* opcodes. */
  int bufPos = 0;
  if ((a & CTC_Space))
    scratchBuf[bufPos++] = 's';
  if ((a & CTC_Letter))
    scratchBuf[bufPos++] = 'l';
  if ((a & CTC_Digit))
    scratchBuf[bufPos++] = 'd';
  if ((a & CTC_Punctuation))
    scratchBuf[bufPos++] = 'p';
  if ((a & CTC_UpperCase))
    scratchBuf[bufPos++] = 'U';
  if ((a & CTC_LowerCase))
    scratchBuf[bufPos++] = 'u';
  if ((a & CTC_Math))
    scratchBuf[bufPos++] = 'm';
  if ((a & CTC_Sign))
    scratchBuf[bufPos++] = 'S';
  if ((a & CTC_LitDigit))
    scratchBuf[bufPos++] = 'D';
  if ((a & CTC_Class1))
    scratchBuf[bufPos++] = 'w';
  if ((a & CTC_Class2))
    scratchBuf[bufPos++] = 'x';
  if ((a & CTC_Class3))
    scratchBuf[bufPos++] = 'y';
  if ((a & CTC_Class4))
    scratchBuf[bufPos++] = 'z';
  scratchBuf[bufPos] = 0;
  return scratchBuf;
}