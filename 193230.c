void DcmSCP::callbackSENDProgress(void *callbackContext,
                                  const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCP *, callbackContext)->notifySENDProgress(byteCount);
}