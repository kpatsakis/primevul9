queue_check_only(void)
{
int sep = 0;
struct stat statbuf;
const uschar *s;
uschar *ss;
uschar buffer[1024];

if (queue_only_file == NULL) return;

s = queue_only_file;
while ((ss = string_nextinlist(&s, &sep, buffer, sizeof(buffer))) != NULL)
  {
  if (Ustrncmp(ss, "smtp", 4) == 0)
    {
    ss += 4;
    if (Ustat(ss, &statbuf) == 0)
      {
      f.queue_smtp = TRUE;
      DEBUG(D_receive) debug_printf("queue_smtp set because %s exists\n", ss);
      }
    }
  else
    {
    if (Ustat(ss, &statbuf) == 0)
      {
      queue_only = TRUE;
      DEBUG(D_receive) debug_printf("queue_only set because %s exists\n", ss);
      }
    }
  }
}