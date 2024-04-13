sigterm_handler (int signum)
{
  write (sigterm_pipefd[1], "X", 1);
}