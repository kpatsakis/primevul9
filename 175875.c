static int formatIPTC(Image *ifile, Image *ofile)
{
  char
    temp[MaxTextExtent];

  unsigned int
    foundiptc,
    tagsfound;

  unsigned char
    recnum,
    dataset;

  unsigned char
    *readable,
    *str;

  ssize_t
    tagindx,
    taglen;

  int
    i,
    tagcount = (int) (sizeof(tags) / sizeof(tag_spec));

  int
    c;

  foundiptc = 0; /* found the IPTC-Header */
  tagsfound = 0; /* number of tags found */

  c = ReadBlobByte(ifile);
  while (c != EOF)
  {
    if (c == 0x1c)
      foundiptc=1;
    else
      {
        if (foundiptc)
          return(-1);
        else
          {
            c=0;
            continue;
          }
      }

    /* we found the 0x1c tag and now grab the dataset and record number tags */
    c = ReadBlobByte(ifile);
    if (c == EOF) return -1;
    dataset = (unsigned char) c;
    c = ReadBlobByte(ifile);
    if (c == EOF) return -1;
    recnum = (unsigned char) c;
    /* try to match this record to one of the ones in our named table */
    for (i=0; i< tagcount; i++)
    {
      if (tags[i].id == (short) recnum)
          break;
    }
    if (i < tagcount)
      readable = (unsigned char *) tags[i].name;
    else
      readable = (unsigned char *) "";
    /*
      We decode the length of the block that follows - ssize_t or short fmt.
    */
    c=ReadBlobByte(ifile);
    if (c == EOF) return -1;
    if (c & (unsigned char) 0x80)
      return 0;
    else
      {
        int
          c0;

        c0=ReadBlobByte(ifile);
        if (c0 == EOF) return -1;
        taglen = (c << 8) | c0;
      }
    if (taglen < 0) return -1;
    /* make a buffer to hold the tag datand snag it from the input stream */
    str=(unsigned char *) AcquireQuantumMemory((size_t) (taglen+MaxTextExtent),
      sizeof(*str));
    if (str == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
    for (tagindx=0; tagindx<taglen; tagindx++)
    {
      c=ReadBlobByte(ifile);
      if (c == EOF) return -1;
      str[tagindx] = (unsigned char) c;
    }
    str[taglen] = 0;

    /* now finish up by formatting this binary data into ASCII equivalent */
    if (strlen((char *)readable) > 0)
      (void) FormatLocaleString(temp,MaxTextExtent,"%d#%d#%s=",
        (unsigned int) dataset, (unsigned int) recnum, readable);
    else
      (void) FormatLocaleString(temp,MaxTextExtent,"%d#%d=",
        (unsigned int) dataset,(unsigned int) recnum);
    (void) WriteBlobString(ofile,temp);
    formatString( ofile, (char *)str, taglen );
    str=(unsigned char *) RelinquishMagickMemory(str);

    tagsfound++;

    c=ReadBlobByte(ifile);
  }
  return((int) tagsfound);
}