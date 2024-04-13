static void terminateAllReceivers(DVConfiguration& dvi)
{
  OFLOG_INFO(dcmpsrcvLogger, "Terminating all receivers");

  const char *recID=NULL;
  const char *recAETitle=NULL;
  unsigned short recPort=0;
  OFBool recUseTLS=OFFalse;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc=NULL;
  OFBool prepared = OFTrue;
  const char *xfer = UID_LittleEndianImplicitTransferSyntax;

#ifdef WITH_OPENSSL
  /* TLS directory */
  const char *current = NULL;
  const char *tlsFolder = dvi.getTLSFolder();
  if (tlsFolder==NULL) tlsFolder = ".";

  /* key file format */
  int keyFileFormat = SSL_FILETYPE_PEM;
  if (! dvi.getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;
#endif

  if ((ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net).bad())) return;

  Uint32 numReceivers = dvi.getNumberOfTargets(DVPSE_receiver);
  for (Uint32 i=0; i<numReceivers; i++)
  {
    prepared = OFTrue;
    recID = dvi.getTargetID(i, DVPSE_receiver);
    recPort = dvi.getTargetPort(recID);
    recUseTLS = dvi.getTargetUseTLS(recID);
    recAETitle = dvi.getTargetAETitle(recID);

    OFLOG_INFO(dcmpsrcvLogger, "Receiver " << recID << " on port " << recPort
         << (recUseTLS ? " with TLS" : ""));

    if (recUseTLS)
    {
#ifdef WITH_OPENSSL
      /* certificate file */
      OFString tlsCertificateFile(tlsFolder);
      tlsCertificateFile += PATH_SEPARATOR;
      current = dvi.getTargetCertificate(recID);
      if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

      /* private key file */
      OFString tlsPrivateKeyFile(tlsFolder);
      tlsPrivateKeyFile += PATH_SEPARATOR;
      current = dvi.getTargetPrivateKey(recID);
      if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

      /* private key password */
      const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(recID);

      /* DH parameter file */
      OFString tlsDHParametersFile;
      current = dvi.getTargetDiffieHellmanParameters(recID);
      if (current)
      {
        tlsDHParametersFile = tlsFolder;
        tlsDHParametersFile += PATH_SEPARATOR;
        tlsDHParametersFile += current;
      }

      /* random seed file */
      OFString tlsRandomSeedFile(tlsFolder);
      tlsRandomSeedFile += PATH_SEPARATOR;
      current = dvi.getTargetRandomSeed(recID);
      if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

      /* CA certificate directory */
      const char *tlsCACertificateFolder = dvi.getTLSCACertificateFolder();
      if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

      /* ciphersuites */
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
      OFString tlsCiphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
      OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
      Uint32 tlsNumberOfCiphersuites = dvi.getTargetNumberOfCipherSuites(recID);
      if (tlsNumberOfCiphersuites > 0)
      {
        tlsCiphersuites.clear();
        OFString currentSuite;
        const char *currentOpenSSL;
        for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
        {
          dvi.getTargetCipherSuite(recID, ui, currentSuite);
          if (NULL != (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
          {
            if (!tlsCiphersuites.empty()) tlsCiphersuites += ":";
            tlsCiphersuites += currentOpenSSL;
          }
        }
      }
      DcmTLSTransportLayer *tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, tlsRandomSeedFile.c_str());
      if (tLayer)
      {
        if (tlsCACertificateFolder) tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat);
        if (tlsDHParametersFile.size() > 0) tLayer->setTempDHParameters(tlsDHParametersFile.c_str());
        tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console
        tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat);
        tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat);
        tLayer->setCipherSuites(tlsCiphersuites.c_str());
        tLayer->setCertificateVerification(DCV_ignoreCertificate);
        ASC_setTransportLayer(net, tLayer, 1);
      }
#else
      prepared = OFFalse;
#endif
    } else {
      DcmTransportLayer *dLayer = new DcmTransportLayer(DICOM_APPLICATION_REQUESTOR);
      ASC_setTransportLayer(net, dLayer, 1);
    }
    if (prepared && recAETitle && (recPort > 0))
    {
      if ((ASC_createAssociationParameters(&params, DEFAULT_MAXPDU)).good())
      {
        ASC_setTransportLayerType(params, recUseTLS);
        ASC_setAPTitles(params, dvi.getNetworkAETitle(), recAETitle, NULL);
        gethostname(localHost, sizeof(localHost) - 1);
        sprintf(peerHost, "%s:%d", "localhost", (int)recPort);
        ASC_setPresentationAddresses(params, localHost, peerHost);
        // we propose only the "shutdown" SOP class in implicit VR
        ASC_addPresentationContext(params, 1, UID_PrivateShutdownSOPClass, &xfer, 1);
        // request shutdown association, abort if some strange peer accepts it
        if (ASC_requestAssociation(net, params, &assoc).good()) ASC_abortAssociation(assoc);
        ASC_dropAssociation(assoc);
        ASC_destroyAssociation(&assoc);
      }
    }
  } /* for loop */

  ASC_dropNetwork(&net);
#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
  return;
}