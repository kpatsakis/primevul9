void DcmSCP::callbackRECEIVEProgress(void *callbackContext,
                                     const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCP *, callbackContext)->notifyRECEIVEProgress(byteCount);
}