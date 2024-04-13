int __xmlInitializeDict(void) {
    if (xmlDictInitialized)
        return(1);

    if ((xmlDictMutex = xmlNewRMutex()) == NULL)
        return(0);
    xmlRMutexLock(xmlDictMutex);

#ifdef DICT_RANDOMIZATION
#ifdef HAVE_RAND_R
    rand_seed = time(NULL);
    rand_r(& rand_seed);
#else
    srand(time(NULL));
#endif
#endif
    xmlDictInitialized = 1;
    xmlRMutexUnlock(xmlDictMutex);
    return(1);
}