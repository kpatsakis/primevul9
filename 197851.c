lou_logEnd ()
{
  if (logFile != NULL)
    fclose (logFile);
  logFile = NULL;
}