static void sigalrm_handler(int sig __attribute__((unused)))
{
    gotsigalrm = 1;
}