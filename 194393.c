static int strToMatch(const char* str ,char *retstr)
{
	char* 	anchor 	= NULL;
	const char* 	anchor1 = NULL;
	int 	result 	= 0;

    if( (!str) || str[0] == '\0'){
        return result;
    } else {
	anchor = retstr;
	anchor1 = str;
        while( (*str)!='\0' ){
		if( *str == '-' ){
			*retstr =  '_';
		} else {
			*retstr = tolower(*str);
		}
            str++;
            retstr++;
	}
	*retstr = '\0';
	retstr=  anchor;
	str=  anchor1;
	result = 1;
    }

    return(result);
}