WlmActivityManager::WlmActivityManager(
    WlmDataSource *dataSourcev,
    OFCmdUnsignedInt opt_portv,
    OFBool opt_refuseAssociationv,
    OFBool opt_rejectWithoutImplementationUIDv,
    OFCmdUnsignedInt opt_sleepAfterFindv,
    OFCmdUnsignedInt opt_sleepDuringFindv,
    OFCmdUnsignedInt opt_maxPDUv,
    E_TransferSyntax opt_networkTransferSyntaxv,
    OFBool opt_failInvalidQueryv,
    OFBool opt_singleProcessv,
    int opt_maxAssociationsv,
    T_DIMSE_BlockingMode opt_blockModev,
    int opt_dimse_timeoutv,
    int opt_acse_timeoutv,
    OFBool opt_forkedChildv,
    int argcv,
    char *argvv[] )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : dataSourcev                         - [in] Pointer to the data source which shall be used.
//                opt_portv                           - [in] The port on which the application is supposed to listen.
//                opt_refuseAssociationv              - [in] Specifies if an association shall always be refused by the SCP.
//                opt_rejectWithoutImplementationUIDv - [in] Specifies if the application shall reject an association if no implementation class UID is provided by the calling SCU.
//                opt_sleepAfterFindv                 - [in] Specifies how many seconds the application is supposed to sleep after having handled a C-FIND-Rsp.
//                opt_sleepDuringFindv                - [in] Specifies how many seconds the application is supposed to sleep during the handling of a C-FIND-Rsp.
//                opt_maxPDUv                         - [in] Maximum length of a PDU that can be received in bytes.
//                opt_networkTransferSyntaxv          - [in] Specifies the preferred network transfer syntaxes.
//                opt_failInvalidQueryv               - [in] Specifies if the application shall fail on an invalid query.
//                opt_singleProcessv                  - [in] Specifies if the application shall run in a single process.
//                opt_maxAssociationsv                - [in] Specifies many concurrent associations the application shall be able to handle.
//                opt_forkedChildv                    - [in] Indicates, whether this process was "forked" from a parent process, default: false
//                argcv                               - [in] Number of arguments in command line
//                argvv                               - [in/out] Holds complete commandline
// Return Value : none.
  : dataSource( dataSourcev ), opt_port( opt_portv ), opt_refuseAssociation( opt_refuseAssociationv ),
    opt_rejectWithoutImplementationUID( opt_rejectWithoutImplementationUIDv ),
    opt_sleepAfterFind( opt_sleepAfterFindv ), opt_sleepDuringFind( opt_sleepDuringFindv ),
    opt_maxPDU( opt_maxPDUv ), opt_networkTransferSyntax( opt_networkTransferSyntaxv ),
    opt_failInvalidQuery( opt_failInvalidQueryv ),
    opt_singleProcess( opt_singleProcessv ),  opt_forkedChild( opt_forkedChildv ), cmd_argc( argcv ),
    cmd_argv( argvv ), opt_maxAssociations( opt_maxAssociationsv ),
    opt_blockMode(opt_blockModev), opt_dimse_timeout(opt_dimse_timeoutv), opt_acse_timeout(opt_acse_timeoutv),
    supportedAbstractSyntaxes( NULL ), numberOfSupportedAbstractSyntaxes( 0 ),
    processTable( )
{
  // initialize supported abstract transfer syntaxes.
  supportedAbstractSyntaxes = new char*[2];
  supportedAbstractSyntaxes[0] = new char[ strlen( UID_VerificationSOPClass ) + 1 ];
  strcpy( supportedAbstractSyntaxes[0], UID_VerificationSOPClass );
  supportedAbstractSyntaxes[1] = new char[ strlen( UID_FINDModalityWorklistInformationModel ) + 1 ];
  strcpy( supportedAbstractSyntaxes[1], UID_FINDModalityWorklistInformationModel );
  numberOfSupportedAbstractSyntaxes = 2;

  // make sure not to let dcmdata remove trailing blank padding or perform other
  // manipulations. We want to see the real data.
  dcmEnableAutomaticInputDataCorrection.set( OFFalse );
  if (!opt_forkedChild)
    DCMWLM_WARN("(notice: dcmdata auto correction disabled.)");

#ifdef HAVE_GUSI_H
  // needed for Macintosh.
  GUSISetup( GUSIwithSIOUXSockets );
  GUSISetup( GUSIwithInternetSockets );
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  // we need at least version 1.1.
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif

}