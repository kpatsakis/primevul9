void ZrtpQueue::srtpSecretsOn(std::string c, std::string s, bool verified)
{

  if (zrtpUserCallback != NULL) {
    zrtpUserCallback->secureOn(c);
    if (!s.empty()) {
        zrtpUserCallback->showSAS(s, verified);
    }
  }
}