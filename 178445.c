get_char_class(char_u **pp)
{
    static const char *(class_names[]) =
    {
	"alnum:]",
#define CLASS_ALNUM 0
	"alpha:]",
#define CLASS_ALPHA 1
	"blank:]",
#define CLASS_BLANK 2
	"cntrl:]",
#define CLASS_CNTRL 3
	"digit:]",
#define CLASS_DIGIT 4
	"graph:]",
#define CLASS_GRAPH 5
	"lower:]",
#define CLASS_LOWER 6
	"print:]",
#define CLASS_PRINT 7
	"punct:]",
#define CLASS_PUNCT 8
	"space:]",
#define CLASS_SPACE 9
	"upper:]",
#define CLASS_UPPER 10
	"xdigit:]",
#define CLASS_XDIGIT 11
	"tab:]",
#define CLASS_TAB 12
	"return:]",
#define CLASS_RETURN 13
	"backspace:]",
#define CLASS_BACKSPACE 14
	"escape:]",
#define CLASS_ESCAPE 15
	"ident:]",
#define CLASS_IDENT 16
	"keyword:]",
#define CLASS_KEYWORD 17
	"fname:]",
#define CLASS_FNAME 18
    };
#define CLASS_NONE 99
    int i;

    if ((*pp)[1] == ':')
    {
	for (i = 0; i < (int)ARRAY_LENGTH(class_names); ++i)
	    if (STRNCMP(*pp + 2, class_names[i], STRLEN(class_names[i])) == 0)
	    {
		*pp += STRLEN(class_names[i]) + 2;
		return i;
	    }
    }
    return CLASS_NONE;
}