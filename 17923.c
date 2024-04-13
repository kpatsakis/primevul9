_XkbCheckTypeName(Atom name,int typeNdx)
{
char *	str;

    str= NameForAtom(name);
    if ((strcmp(str,"ONE_LEVEL")==0)||(strcmp(str,"TWO_LEVEL")==0)||
	(strcmp(str,"ALPHABETIC")==0)||(strcmp(str,"KEYPAD")==0))
	return False;
    return True;
}