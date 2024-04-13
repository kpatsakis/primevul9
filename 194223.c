APE::Properties::Properties(File *file, ReadStyle style) : AudioProperties(style)
{
  d = new PropertiesPrivate(file, file->length());
  read();
}