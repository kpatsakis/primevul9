FileSpec::~FileSpec()
{
  delete fileName;
  delete platformFileName;
  delete embFile;
  delete desc;
}