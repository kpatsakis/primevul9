static void nwfilterDriverLock(void)
{
    virMutexLock(&driver->lock);
}