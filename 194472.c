static void *DestroyLogElement(void *log_info)
{
  register LogInfo
    *p;

  p=(LogInfo *) log_info;
  if (p->file != (FILE *) NULL)
    {
      (void) FormatLocaleFile(p->file,"</log>\n");
      (void) fclose(p->file);
      p->file=(FILE *) NULL;
    }
  if (p->format != (char *) NULL)
    p->format=DestroyString(p->format);
  if (p->path != (char *) NULL)
    p->path=DestroyString(p->path);
  if (p->filename != (char *) NULL)
    p->filename=DestroyString(p->filename);
  p=(LogInfo *) RelinquishMagickMemory(p);
  return((void *) NULL);
}