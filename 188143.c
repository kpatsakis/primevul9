MagickExport MagickBooleanType ListDelegateInfo(FILE *file,
  ExceptionInfo *exception)
{
  const DelegateInfo
    **delegate_info;

  char
    **commands,
    delegate[MagickPathExtent];

  const char
    *path;

  register ssize_t
    i;

  size_t
    number_delegates;

  ssize_t
    j;

  if (file == (const FILE *) NULL)
    file=stdout;
  delegate_info=GetDelegateInfoList("*",&number_delegates,exception);
  if (delegate_info == (const DelegateInfo **) NULL)
    return(MagickFalse);
  path=(const char *) NULL;
  for (i=0; i < (ssize_t) number_delegates; i++)
  {
    if (delegate_info[i]->stealth != MagickFalse)
      continue;
    if ((path == (const char *) NULL) ||
        (LocaleCompare(path,delegate_info[i]->path) != 0))
      {
        if (delegate_info[i]->path != (char *) NULL)
          (void) FormatLocaleFile(file,"\nPath: %s\n\n",delegate_info[i]->path);
        (void) FormatLocaleFile(file,"Delegate                Command\n");
        (void) FormatLocaleFile(file,
          "-------------------------------------------------"
          "------------------------------\n");
      }
    path=delegate_info[i]->path;
    *delegate='\0';
    if (delegate_info[i]->encode != (char *) NULL)
      (void) CopyMagickString(delegate,delegate_info[i]->encode,
        MagickPathExtent);
    (void) ConcatenateMagickString(delegate,"        ",MagickPathExtent);
    delegate[8]='\0';
    commands=StringToList(delegate_info[i]->commands);
    if (commands == (char **) NULL)
      continue;
    (void) FormatLocaleFile(file,"%11s%c=%c%s  ",delegate_info[i]->decode ?
      delegate_info[i]->decode : "",delegate_info[i]->mode <= 0 ? '<' : ' ',
      delegate_info[i]->mode >= 0 ? '>' : ' ',delegate);
    StripString(commands[0]);
    (void) FormatLocaleFile(file,"\"%s\"\n",commands[0]);
    for (j=1; commands[j] != (char *) NULL; j++)
    {
      StripString(commands[j]);
      (void) FormatLocaleFile(file,"                     \"%s\"\n",commands[j]);
    }
    for (j=0; commands[j] != (char *) NULL; j++)
      commands[j]=DestroyString(commands[j]);
    commands=(char **) RelinquishMagickMemory(commands);
  }
  (void) fflush(file);
  delegate_info=(const DelegateInfo **)
    RelinquishMagickMemory((void *) delegate_info);
  return(MagickTrue);
}