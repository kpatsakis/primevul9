static void TextToUTF8 (void *string, int len)
{
    unsigned char *inp = string;

    // simple case: test for UTF8 BOM and if so, simply delete the BOM

    if (len > 3 && inp [0] == 0xEF && inp [1] == 0xBB && inp [2] == 0xBF) {
        memmove (inp, inp + 3, len - 3);
        inp [len - 3] = 0;
    }
    else if (* (wchar_t *) string == 0xFEFF) {
        wchar_t *temp = _wcsdup (string);

        WideCharToUTF8 (temp + 1, (unsigned char *) string, len);
        free (temp);
    }
    else {
        int max_chars = (int) strlen (string);
        wchar_t *temp = (wchar_t *) malloc ((max_chars + 1) * 2);

        MultiByteToWideChar (CP_ACP, 0, string, -1, temp, max_chars + 1);
        WideCharToUTF8 (temp, (unsigned char *) string, len);
        free (temp);
    }
}