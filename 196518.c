void LibRaw::setCancelFlag()
{
#ifdef WIN32
  InterlockedExchangeAdd(&_exitflag,1);
#else
  __sync_fetch_and_add(&_exitflag,1);
#endif
#ifdef RAWSPEED_FASTEXIT
  if(_rawspeed_decoder)
    {
      RawDecoder *d = static_cast<RawDecoder*>(_rawspeed_decoder);
      d->cancelProcessing();
    }
#endif
}