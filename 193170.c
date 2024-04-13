static void executeCommand( const OFString &cmd )
    /*
     * This function executes the given command line. The execution will be
     * performed in a new process which can be run in the background
     * so that it does not slow down the execution of storescp.
     *
     * Parameters:
     *   cmd - [in] The command which shall be executed.
     */
{
#ifdef HAVE_FORK
  pid_t pid = fork();
  if( pid < 0 )     // in case fork failed, dump an error message
    OFLOG_ERROR(storescpLogger, "cannot execute command '" << cmd << "' (fork failed)");
  else if (pid > 0)
  {
    /* we are the parent process */
    /* remove pending zombie child processes */
    cleanChildren(pid, opt_execSync);
  }
  else // in case we are the child process, execute the command etc.
  {
    // execute command through execl will terminate the child process.
    // Since we only have a single command string and not a list of arguments,
    // we 'emulate' a call to system() by passing the command to /bin/sh
    // which hopefully exists on all Posix systems.

    if (execl( "/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL ) < 0)
      OFLOG_ERROR(storescpLogger, "cannot execute /bin/sh");

    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);

  // execute command (Attention: Do not pass DETACHED_PROCESS as sixth argument to the below
  // called function because in such a case the execution of batch-files is not going to work.)
  if( !CreateProcess(NULL, OFconst_cast(char *, cmd.c_str()), NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo) )
    OFLOG_ERROR(storescpLogger, "cannot execute command '" << cmd << "'");

  if (opt_execSync)
  {
    // Wait until child process exits (makes execution synchronous)
    WaitForSingleObject(procinfo.hProcess, INFINITE);
  }

  // Close process and thread handles to avoid resource leak
  CloseHandle(procinfo.hProcess);
  CloseHandle(procinfo.hThread);
#endif
}