JPXStream::JPXStream(Stream *strA) : FilterStream(strA) {
  priv = new JPXStreamPrivate;
  priv->inited = false;
  priv->image = nullptr;
  priv->npixels = 0;
  priv->ncomps = 0;
}