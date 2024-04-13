static CodeRange read_vrotate(
    int count,
    char **value,
    int *type,
    int *vrotate_num)
{
    CodeRange   range;
    if(!strcmp(value[0],"all")){
	*type 	     = VROTATE_ALL ;
	*vrotate_num = 0 ;
	return (NULL);
    } else if(*(value[0]) == '['){
	*type 	     = VROTATE_PART ;
        range = (CodeRange) _XlcParse_scopemaps(value[0],vrotate_num);
	return (range);
    } else {
	*type 	     = VROTATE_NONE ;
	*vrotate_num = 0 ;
	return (NULL);
    }
}