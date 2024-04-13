reserveSpaceInTable (FileInfo * nested, int count)
{
  return (allocateSpaceInTable (nested, NULL, count));
}