JPXStream::~JPXStream() {
  delete str;
  close();
  delete priv;
}