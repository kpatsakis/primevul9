extern "C" void sigChildHandler(int)
#endif
{
  int status = 0;
  waitpid( -1, &status, WNOHANG );
  signal(SIGCHLD, sigChildHandler);
}