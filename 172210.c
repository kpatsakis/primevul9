Goffset JPXStream::getPos() {
  return priv->counter * priv->ncomps + priv->ccounter;
}