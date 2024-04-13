static void executeOnEndOfStudy()
    /*
     * This function deals with the execution of the command line which was passed
     * to option --exec-on-eostudy of the storescp. This command line is captured
     * in opt_execOnEndOfStudy. Note that the command line can contain the placeholder
     * PATH_PLACEHOLDER which needs to be substituted before the command line is actually executed.
     * In detail, PATH_PLACEHOLDER will be substituted by the path to the output directory into which
     * the files of the last study were written.
     *
     * Parameters:
     *   none.
     */
{
  OFString cmd = opt_execOnEndOfStudy;

  // perform substitution for placeholder #p; #p will be substituted by lastStudySubdirectoryPathAndName
  cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), lastStudySubdirectoryPathAndName );

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? callingAETitle : lastCallingAETitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? calledAETitle : lastCalledAETitle );

  // perform substitution for placeholder #r
  cmd = replaceChars( cmd, OFString(CALLING_PRESENTATION_ADDRESS_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? callingPresentationAddress : lastCallingPresentationAddress );

  // Execute command in a new process
  executeCommand( cmd );
}