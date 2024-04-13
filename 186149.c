static char *EscapeParenthesis(const char *source)
{
  char
    *destination;

  register char
    *q;

  register const char
    *p;

  size_t
    length;

  assert(source != (const char *) NULL);
  length=0;
  for (p=source; *p != '\0'; p++)
  {
    if ((*p == '\\') || (*p == '(') || (*p == ')'))
      {
        if (~length < 1)
          ThrowFatalException(ResourceLimitFatalError,"UnableToEscapeString");
        length++;
      }
    length++;
  }
  destination=(char *) NULL;
  if (~length >= (MaxTextExtent-1))
    destination=(char *) AcquireQuantumMemory(length+MaxTextExtent,
      sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToEscapeString");
  *destination='\0';
  q=destination;
  for (p=source; *p != '\0'; p++)
  {
    if ((*p == '\\') || (*p == '(') || (*p == ')'))
      *q++='\\';
    *q++=(*p);
  }
  *q='\0';
  return(destination);
}