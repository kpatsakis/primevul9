lou_registerTableResolver (char ** (* resolver) (const char *tableList, const char *base))
{
  tableResolver = resolver;
}