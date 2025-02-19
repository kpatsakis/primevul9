int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    dcmDisableGethostbyaddr.set(OFTrue);  // disable hostname lookup

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM basic grayscale print management SCP", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addGroup("general options:");
     cmd.addOption("--help",    "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",          "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);
     cmd.addOption("--logfile", "-l",    "write a log file (not with --log-config)");

    cmd.addGroup("processing options:");
     cmd.addOption("--config",  "-c", 1, "[f]ilename: string",
                                         "process using settings from configuration file");
     cmd.addOption("--printer", "-p", 1, "[n]ame: string (default: 1st printer in config file)",
                                         "select printer with identifier n from config file");
     cmd.addOption("--dump",    "+d",    "dump all DIMSE messages");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            COUT << " none" << OFendl;
#else
            COUT << OFendl;
#endif
#ifdef WITH_ZLIB
            COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_OPENSSL
            COUT << "- " << OPENSSL_VERSION_TEXT << OFendl;
#endif
            return 0;
         }
      }

      /* options */
      if (cmd.findOption("--dump"))
      {
        // Messages to the "dump" logger are always written with the debug log
        // level, thus enabling that logger for this level shows the dumps
        dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getInstance("dcmtk.dcmpstat.dump");
        log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
      }

      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--logfile"))
      {
        app.checkConflict("--logfile", "--log-config", cmd.findOption("--log-config"));
        opt_logFile = OFTrue;
      }
      if (cmd.findOption("--config"))  app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer")) app.checkValue(cmd.getValue(opt_printer));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmprscpLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmprscpLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    } else {
        OFLOG_FATAL(dcmprscpLogger, "no configuration file specified");
        return 10;
    }

    DVInterface dvi(opt_cfgName);

    if (opt_printer)
    {
      if (DVPSE_printLocal != dvi.getTargetType(opt_printer))
      {
        OFLOG_FATAL(dcmprscpLogger, "no print scp definition for '" << opt_printer << "' found in config file");
        return 10;
      }
    } else {
      opt_printer = dvi.getTargetID(0, DVPSE_printLocal); // use default print scp
      if (opt_printer==NULL)
      {
        OFLOG_FATAL(dcmprscpLogger, "no default print scp available - no config file?");
        return 10;
      }
    }

    opt_binaryLog = dvi.getBinaryLog();

    OFString logfileprefix;
    OFString aString;
    unsigned long logcounter = 0;
    char logcounterbuf[20];

    logfileprefix = dvi.getSpoolFolder();

    logfileprefix += PATH_SEPARATOR;
    logfileprefix += "PrintSCP_";
    logfileprefix += opt_printer;
    logfileprefix += "_";
    DVPSHelper::currentDate(aString);
    logfileprefix += aString;
    logfileprefix += "_";
    DVPSHelper::currentTime(aString);
    logfileprefix += aString;

    if (opt_logFile)
    {
      const char *pattern = "%m%n";
      OFString logfilename = logfileprefix;
      logfilename += ".log";

      OFauto_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout(pattern));
      dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender(logfilename));
      dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();

      logfile->setLayout(layout);
      log.removeAllAppenders();
      log.addAppender(logfile);
    }

    OFLOG_WARN(dcmprscpLogger, rcsid << OFendl << OFDateTime::getCurrentDateTime() << OFendl << "started");

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
       OFLOG_WARN(dcmprscpLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();
    OFLOG_INFO(dcmprscpLogger, "Using database in directory '" << dbfolder << "'");

    OFCondition cond2 = EC_Normal;
    DcmQueryRetrieveIndexDatabaseHandle *dbhandle = new DcmQueryRetrieveIndexDatabaseHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, cond2);
    delete dbhandle;

    if (cond2.bad())
    {
      OFLOG_FATAL(dcmprscpLogger, "Unable to access database '" << dbfolder << "'");
      return 10;
    }

    /* get print scp data from configuration file */
    unsigned short targetPort   = dvi.getTargetPort(opt_printer);
    OFBool targetDisableNewVRs  = dvi.getTargetDisableNewVRs(opt_printer);
    OFBool targetUseTLS         = dvi.getTargetUseTLS(opt_printer);

    if (targetPort == 0)
    {
      OFLOG_FATAL(dcmprscpLogger, "no or invalid port number for print scp '" << opt_printer << "'");
      return 10;
    }

    if (targetDisableNewVRs)
    {
      dcmEnableUnknownVRGeneration.set(OFFalse);
      dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
      dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
    }

    T_ASC_Network *net = NULL; /* the DICOM network and listen port */
    OFBool finished = OFFalse;
    int connected = 0;

#ifdef WITH_OPENSSL
    /* TLS directory */
    const char *current = NULL;
    const char *tlsFolder = dvi.getTLSFolder();
    if (tlsFolder==NULL) tlsFolder = ".";

    /* certificate file */
    OFString tlsCertificateFile(tlsFolder);
    tlsCertificateFile += PATH_SEPARATOR;
    current = dvi.getTargetCertificate(opt_printer);
    if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

    /* private key file */
    OFString tlsPrivateKeyFile(tlsFolder);
    tlsPrivateKeyFile += PATH_SEPARATOR;
    current = dvi.getTargetPrivateKey(opt_printer);
    if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

    /* private key password */
    const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(opt_printer);

    /* certificate verification */
    DcmCertificateVerification tlsCertVerification = DCV_requireCertificate;
    switch (dvi.getTargetPeerAuthentication(opt_printer))
    {
      case DVPSQ_require:
        tlsCertVerification = DCV_requireCertificate;
        break;
      case DVPSQ_verify:
        tlsCertVerification = DCV_checkCertificate;
        break;
      case DVPSQ_ignore:
        tlsCertVerification = DCV_ignoreCertificate;
        break;
    }

    /* DH parameter file */
    OFString tlsDHParametersFile;
    current = dvi.getTargetDiffieHellmanParameters(opt_printer);
    if (current)
    {
      tlsDHParametersFile = tlsFolder;
      tlsDHParametersFile += PATH_SEPARATOR;
      tlsDHParametersFile += current;
    }

    /* random seed file */
    OFString tlsRandomSeedFile(tlsFolder);
    tlsRandomSeedFile += PATH_SEPARATOR;
    current = dvi.getTargetRandomSeed(opt_printer);
    if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

    /* CA certificate directory */
    const char *tlsCACertificateFolder = dvi.getTLSCACertificateFolder();
    if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

    /* key file format */
    int keyFileFormat = SSL_FILETYPE_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;

    /* ciphersuites */
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    OFString tlsCiphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
    OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
    Uint32 tlsNumberOfCiphersuites = dvi.getTargetNumberOfCipherSuites(opt_printer);
    if (tlsNumberOfCiphersuites > 0)
    {
      tlsCiphersuites.clear();
      OFString currentSuite;
      const char *currentOpenSSL;
      for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
      {
        dvi.getTargetCipherSuite(opt_printer, ui, currentSuite);
        if (NULL == (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
        {
          OFLOG_WARN(dcmprscpLogger, "ciphersuite '" << currentSuite << "' is unknown. Known ciphersuites are:");
          unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
          for (unsigned long cs=0; cs < numSuites; cs++)
          {
            OFLOG_WARN(dcmprscpLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
          }
          return 1;
        } else {
          if (!tlsCiphersuites.empty()) tlsCiphersuites += ":";
          tlsCiphersuites += currentOpenSSL;
        }
      }
    }

    DcmTLSTransportLayer *tLayer = NULL;
    if (targetUseTLS)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_ACCEPTOR, tlsRandomSeedFile.c_str());
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to create TLS transport layer");
        return 1;
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        OFLOG_WARN(dcmprscpLogger, "unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring");
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        OFLOG_WARN(dcmprscpLogger, "unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring");
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'");
        return 1;
      }
      if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to load certificate from '" << tlsCertificateFile << "'");
        return 1;
      }
      if (! tLayer->checkPrivateKeyMatchesCertificate())
      {
        OFLOG_FATAL(dcmprscpLogger, "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match");
        return 1;
      }
      if (TCS_ok != tLayer->setCipherSuites(tlsCiphersuites.c_str()))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(tlsCertVerification);

    }
#else
    if (targetUseTLS)
    {
      OFLOG_FATAL(dcmprscpLogger, "not compiled with OpenSSL, cannot use TLS");
      return 10;
    }
#endif

    /* open listen socket */
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, targetPort, 30, &net);
    if (cond.bad())
    {
      OFString temp_str;
      OFLOG_FATAL(dcmprscpLogger, "cannot initialise network:\n" << DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
        OFString temp_str;
        OFLOG_FATAL(dcmprscpLogger, DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }
#endif

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
    /* return to normal uid so that we can't do too much damage in case
     * things go very wrong.   Only relevant if the program is setuid root,
     * and run by another user.  Running as root user may be
     * potentially disasterous if this program screws up badly.
     */
    if ((setuid(getuid()) == -1) && (errno == EAGAIN))
    {
        OFLOG_FATAL(dcmprscpLogger, "setuid() failed, maximum number of processes/threads for uid already running.");
        return 1;
    }
#endif

#ifdef HAVE_FORK
    int timeout=1;
#else
    int timeout=1000;
#endif

    while (!finished)
    {
      DVPSPrintSCP printSCP(dvi, opt_printer); // use new print SCP object for each association

      if (opt_binaryLog)
      {
        aString = logfileprefix;
        aString += "_";
        sprintf(logcounterbuf, "%04ld", ++logcounter);
        aString += logcounterbuf;
        aString += ".dcm";
        printSCP.setDimseLogPath(aString.c_str());
      }

      connected = 0;
      while (!connected)
      {
        connected = ASC_associationWaiting(net, timeout);
        if (!connected) cleanChildren();
      }
      switch (printSCP.negotiateAssociation(*net))
      {
        case DVPSJ_error:
          // association has already been deleted, we just wait for the next client to connect.
          break;
        case DVPSJ_terminate:
          finished=OFTrue;
          cond = ASC_dropNetwork(&net);
          if (cond.bad())
          {
            OFString temp_str;
            OFLOG_FATAL(dcmprscpLogger, "cannot drop network:\n" << DimseCondition::dump(temp_str, cond));
            return 10;
          }
          break;
        case DVPSJ_success:
          printSCP.handleClient();
          break;
      }
    } // finished
    cleanChildren();

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(tlsRandomSeedFile.c_str()))
        {
          OFLOG_ERROR(dcmprscpLogger, "cannot write back random seed file '" << tlsRandomSeedFile << "', ignoring");
        }
      } else {
        OFLOG_WARN(dcmprscpLogger, "cannot write back random seed, ignoring");
      }
    }
    delete tLayer;
#endif

    return 0;
}