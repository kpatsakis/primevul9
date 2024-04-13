static int fits_scanfdouble (const char *buf, double *value)
{
 int retval = 0;
 gchar *bufcopy = g_strdup (buf);

 /* We should use g_ascii_strtod. This also allows scanning of hexadecimal */
 /* values like 0x02. But we want the behaviour of sscanf ("0x02","%lf",...*/
 /* that gives 0.0 in this case. So check the string if we have a hex-value*/

 if ( bufcopy )
 {
   gchar *bufptr = bufcopy;

   /* Remove leading white space */
   g_strchug (bufcopy);

   /* Skip leading sign character */
   if ( (*bufptr == '-') || (*bufptr == '+') )
     bufptr++;

   /* Start of hex value ? Take this as 0.0 */
   if ( (bufptr[0] == '0') && (g_ascii_toupper (bufptr[1]) == 'X') )
   {
     *value = 0.0;
     retval = 1;
   }
   else
   {
     if ( *bufptr == '.' ) /* leading decimal point ? Skip it */
       bufptr++;

     if (g_ascii_isdigit (*bufptr)) /* Expect the complete string is decimal */
     {
       gchar *endptr;
       gdouble gvalue = g_ascii_strtod (bufcopy, &endptr);
       if ( errno == 0 )
       {
         *value = gvalue;
         retval = 1;
       }
     }
   }
   g_free (bufcopy);
 }
 return retval;
}