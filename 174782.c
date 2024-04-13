    BasicIo::AutoPtr ImageFactory::createIo(const std::string& path, bool useCurl)
    {
        Protocol fProt = fileProtocol(path);
#if EXV_USE_SSH == 1
        if (fProt == pSsh || fProt == pSftp) {
            return BasicIo::AutoPtr(new SshIo(path)); // may throw
        }
#endif
#if EXV_USE_CURL == 1
        if (useCurl && (fProt == pHttp || fProt == pHttps || fProt == pFtp)) {
            return BasicIo::AutoPtr(new CurlIo(path)); // may throw
        }
#endif
        if (fProt == pHttp)
            return BasicIo::AutoPtr(new HttpIo(path)); // may throw
        if (fProt == pFileUri)
            return BasicIo::AutoPtr(new FileIo(pathOfFileUrl(path)));
        if (fProt == pStdin || fProt == pDataUri)
            return BasicIo::AutoPtr(new XPathIo(path)); // may throw

        return BasicIo::AutoPtr(new FileIo(path));

        (void)(useCurl);
    } // ImageFactory::createIo