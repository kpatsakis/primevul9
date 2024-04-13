getAChar (FileInfo * nested)
{
/*Read a big endian, little *ndian or ASCII 8 file and convert it to 
* 16- or 32-bit unsigned integers */
  int ch1 = 0, ch2 = 0;
  widechar character;
  if (nested->encoding == ascii8)
    if (nested->status == 2)
      {
	nested->status++;
	return nested->checkencoding[1];
      }
  while ((ch1 = fgetc (nested->in)) != EOF)
    {
      if (nested->status < 2)
	nested->checkencoding[nested->status] = ch1;
      nested->status++;
      if (nested->status == 2)
	{
	  if (nested->checkencoding[0] == 0xfe
	      && nested->checkencoding[1] == 0xff)
	    nested->encoding = bigEndian;
	  else if (nested->checkencoding[0] == 0xff
		   && nested->checkencoding[1] == 0xfe)
	    nested->encoding = littleEndian;
	  else if (nested->checkencoding[0] < 128
		   && nested->checkencoding[1] < 128)
	    {
	      nested->encoding = ascii8;
	      return nested->checkencoding[0];
	    }
	  else
	    {
	      compileError (nested,
			    "encoding is neither big-endian, little-endian nor ASCII 8.");
	      ch1 = EOF;
	      break;;
	    }
	  continue;
	}
      switch (nested->encoding)
	{
	case noEncoding:
	  break;
	case ascii8:
	  return ch1;
	  break;
	case bigEndian:
	  ch2 = fgetc (nested->in);
	  if (ch2 == EOF)
	    break;
	  character = (ch1 << 8) | ch2;
	  return (int) character;
	  break;
	case littleEndian:
	  ch2 = fgetc (nested->in);
	  if (ch2 == EOF)
	    break;
	  character = (ch2 << 8) | ch1;
	  return (int) character;
	  break;
	}
      if (ch1 == EOF || ch2 == EOF)
	break;
    }
  return EOF;
}