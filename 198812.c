int main (int argc, char **argv)
{
  ADDRESS *list;
  char buf[256];
# if 0
  char *str = "michael, Michael Elkins <me@mutt.org>, testing a really complex address: this example <@contains.a.source.route,@with.multiple.hosts:address@example.com>;, lothar@of.the.hillpeople (lothar)";
# else
  char *str = "a b c ";
# endif

  list = rfc822_parse_adrlist (NULL, str);
  buf[0] = 0;
  rfc822_write_address (buf, sizeof (buf), list);
  rfc822_free_address (&list);
  puts (buf);
  exit (0);
}