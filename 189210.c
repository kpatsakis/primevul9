ZrtpQueue::initialize(const char *zidFilename, bool autoEnable, ZrtpConfigure* config)
{
    int32_t ret = 1;

    synchEnter();

    ZrtpConfigure* configOwn = NULL;
    if (config == NULL) {
        config = configOwn = new ZrtpConfigure();
        config->setStandardConfig();
    }
    enableZrtp = autoEnable;

    config->setParanoidMode(enableParanoidMode);

    if (staticTimeoutProvider == NULL) {
        staticTimeoutProvider = new TimeoutProvider<std::string, ZrtpQueue*>();
        staticTimeoutProvider->start();
    }
    ZIDCache* zf = getZidCacheInstance();
    if (!zf->isOpen()) {
        std::string fname;
        if (zidFilename == NULL) {
            char *home = getenv("HOME");
            std::string baseDir = (home != NULL) ? (std::string(home) + std::string("/."))
                                                    : std::string(".");
            fname = baseDir + std::string("GNUZRTP.zid");
            zidFilename = fname.c_str();
        }
        if (zf->open((char *)zidFilename) < 0) {
            enableZrtp = false;
            ret = -1;
        }
    }
    if (ret > 0) {
        const uint8_t* ownZid = zf->getZid();
        zrtpEngine = new ZRtp((uint8_t*)ownZid, (ZrtpCallback*)this, clientIdString, config, mitmMode, signSas);
    }
    if (configOwn != NULL) {
        delete configOwn;
    }
    synchLeave();
    return ret;
}