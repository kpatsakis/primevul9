vim_ispathsep(int c)
{
#ifdef UNIX
    return (c == '/');	    // UNIX has ':' inside file names
#else
# ifdef BACKSLASH_IN_FILENAME
    return (c == ':' || c == '/' || c == '\\');
# else
#  ifdef VMS
    // server"user passwd"::device:[full.path.name]fname.extension;version"
    return (c == ':' || c == '[' || c == ']' || c == '/'
	    || c == '<' || c == '>' || c == '"' );
#  else
    return (c == ':' || c == '/');
#  endif // VMS
# endif
#endif
}