static int _zzip_fnmatch(char* pattern, char* string, int flags)
{ 
    puts ("<zzip:mmapped:strcmp>");
    return strcmp (pattern, string); 
}