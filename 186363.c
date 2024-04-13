int __xmlRandom(void) {
    int ret;

    if (xmlDictInitialized == 0)
        __xmlInitializeDict();

    xmlRMutexLock(xmlDictMutex);
#ifdef HAVE_RAND_R
    ret = rand_r(& rand_seed);
#else
    ret = rand();
#endif
    xmlRMutexUnlock(xmlDictMutex);
    return(ret);
}