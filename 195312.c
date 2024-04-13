Mat_VarWriteData(mat_t *mat,matvar_t *matvar,void *data,
      int *start,int *stride,int *edge)
{
    Mat_Critical("Mat_VarWriteInfo/Mat_VarWriteData is not supported. "
        "Use %s instead!", mat->version == MAT_FT_MAT73 ?
        "Mat_VarWrite/Mat_VarWriteAppend" : "Mat_VarWrite");
    return 1;
}