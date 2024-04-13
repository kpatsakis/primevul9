static OFString replaceChars( const OFString &srcstr, const OFString &pattern, const OFString &substitute )
    /*
     * This function replaces all occurrences of pattern in srcstr with substitute and returns
     * the result as a new OFString variable. Note that srcstr itself will not be changed,
     *
     * Parameters:
     *   srcstr     - [in] The source string.
     *   pattern    - [in] The pattern string which shall be substituted.
     *   substitute - [in] The substitute for pattern in srcstr.
     */
{
  OFString result = srcstr;
  size_t pos = 0;

  while( pos != OFString_npos )
  {
    pos = result.find( pattern, pos );

    if( pos != OFString_npos )
    {
      result.replace( pos, pattern.size(), substitute );
      pos += substitute.size();
    }
  }

  return( result );
}