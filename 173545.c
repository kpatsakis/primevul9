static inline void ConvertCMYKToRGB(PixelInfo *pixel)
{
  pixel->red=((QuantumRange-(QuantumScale*pixel->red*(QuantumRange-
    pixel->black)+pixel->black)));
  pixel->green=((QuantumRange-(QuantumScale*pixel->green*(QuantumRange-
    pixel->black)+pixel->black)));
  pixel->blue=((QuantumRange-(QuantumScale*pixel->blue*(QuantumRange-
    pixel->black)+pixel->black)));
}