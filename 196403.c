static void nwfilterDriverUnlock(void)
{
    virMutexUnlock(&driver->lock);
}