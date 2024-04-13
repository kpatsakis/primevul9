_my_wcslen (int16x * str)
{
int len = 0;
while (*str++ != 0)
  len++;
return len;
}