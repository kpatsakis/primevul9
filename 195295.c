Mat_VarWriteInfo(mat_t *mat, matvar_t *matvar )
{
    Mat_Critical("Mat_VarWriteInfo/Mat_VarWriteData is not supported. "
        "Use %s instead!", mat->version == MAT_FT_MAT73 ?
        "Mat_VarWrite/Mat_VarWriteAppend" : "Mat_VarWrite");
    return 1;
}