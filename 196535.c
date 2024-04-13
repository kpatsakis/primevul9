void LibRaw::checkCancel()
{
#ifdef WIN32
  if(InterlockedExchangeAdd(&_exitflag,0))
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
#else
  if( __sync_add_and_fetch(&_exitflag,0))
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
#endif
}