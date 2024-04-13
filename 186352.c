xmlDictReference(xmlDictPtr dict) {
    if (!xmlDictInitialized)
        if (!__xmlInitializeDict())
            return(-1);

    if (dict == NULL) return -1;
    xmlRMutexLock(xmlDictMutex);
    dict->ref_counter++;
    xmlRMutexUnlock(xmlDictMutex);
    return(0);
}