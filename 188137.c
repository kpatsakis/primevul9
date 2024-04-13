MagickExport int ExternalDelegateCommand(const MagickBooleanType asynchronous,
  const MagickBooleanType verbose,const char *command,char *message,
  ExceptionInfo *exception)
{
  char
    **arguments,
    *sanitize_command;

  int
    number_arguments,
    status;

  PolicyDomain
    domain;

  PolicyRights
    rights;

  register ssize_t
    i;

  status=(-1);
  arguments=StringToArgv(command,&number_arguments);
  if (arguments == (char **) NULL)
    return(status);
  if (*arguments[1] == '\0')
    {
      for (i=0; i < (ssize_t) number_arguments; i++)
        arguments[i]=DestroyString(arguments[i]);
      arguments=(char **) RelinquishMagickMemory(arguments);
      return(-1);
    }
  rights=ExecutePolicyRights;
  domain=DelegatePolicyDomain;
  if (IsRightsAuthorized(domain,rights,arguments[1]) == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",arguments[1]);
      for (i=0; i < (ssize_t) number_arguments; i++)
        arguments[i]=DestroyString(arguments[i]);
      arguments=(char **) RelinquishMagickMemory(arguments);
      return(-1);
    }
  if (verbose != MagickFalse)
    {
      (void) FormatLocaleFile(stderr,"%s\n",command);
      (void) fflush(stderr);
    }
  sanitize_command=SanitizeString(command);
  if (asynchronous != MagickFalse)
    (void) ConcatenateMagickString(sanitize_command,"&",MagickPathExtent);
  if (message != (char *) NULL)
    *message='\0';
#if defined(MAGICKCORE_POSIX_SUPPORT)
#if !defined(MAGICKCORE_HAVE_EXECVP)
  status=system(sanitize_command);
#else
  if ((asynchronous != MagickFalse) ||
      (strpbrk(sanitize_command,"&;<>|") != (char *) NULL))
    status=system(sanitize_command);
  else
    {
      pid_t
        child_pid;

      /*
        Call application directly rather than from a shell.
      */
      child_pid=(pid_t) fork();
      if (child_pid == (pid_t) -1)
        status=system(sanitize_command);
      else
        if (child_pid == 0)
          {
            status=execvp(arguments[1],arguments+1);
            _exit(1);
          }
        else
          {
            int
              child_status;

            pid_t
              pid;

            child_status=0;
            pid=(pid_t) waitpid(child_pid,&child_status,0);
            if (pid == -1)
              status=(-1);
            else
              {
                if (WIFEXITED(child_status) != 0)
                  status=WEXITSTATUS(child_status);
                else
                  if (WIFSIGNALED(child_status))
                    status=(-1);
              }
          }
    }
#endif
#elif defined(MAGICKCORE_WINDOWS_SUPPORT)
  {
    register char
      *p;

    /*
      If a command shell is executed we need to change the forward slashes in
      files to a backslash. We need to do this to keep Windows happy when we
      want to 'move' a file.

      TODO: This won't work if one of the delegate parameters has a forward
            slash as aparameter.
    */
    p=strstr(sanitize_command, "cmd.exe /c");
    if (p != (char*) NULL)
      {
        p+=10;
        for (; *p != '\0'; p++)
          if (*p == '/')
            *p=*DirectorySeparator;
      }
  }
  status=NTSystemCommand(sanitize_command,message);
#elif defined(macintosh)
  status=MACSystemCommand(sanitize_command);
#elif defined(vms)
  status=system(sanitize_command);
#else
#  error No suitable system() method.
#endif
  if (status < 0)
    {
      if ((message != (char *) NULL) && (*message != '\0'))
        (void) ThrowMagickException(exception,GetMagickModule(),DelegateError,
          "FailedToExecuteCommand","`%s' (%s)",sanitize_command,message);
      else
        (void) ThrowMagickException(exception,GetMagickModule(),DelegateError,
          "FailedToExecuteCommand","`%s' (%d)",sanitize_command,status);
    }
  sanitize_command=DestroyString(sanitize_command);
  for (i=0; i < (ssize_t) number_arguments; i++)
    arguments[i]=DestroyString(arguments[i]);
  arguments=(char **) RelinquishMagickMemory(arguments);
  return(status);
}