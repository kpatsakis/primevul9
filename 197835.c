compileFile (const char *fileName)
{
  FileInfo nested;
  fileCount++;
  nested.fileName = fileName;
  nested.encoding = noEncoding;
  nested.status = 0;
  nested.lineNumber = 0;
  if ((nested.in = fopen (nested.fileName, "rb")))
    {
      while (getALine (&nested))
	compileRule (&nested);
      fclose (nested.in);
      return 1;
    }
  else
    lou_logPrint ("Cannot open table '%s'", nested.fileName);
  errorCount++;
  return 0;
}