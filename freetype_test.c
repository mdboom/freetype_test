#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <stdio.h>

int
main(int argc, char** argv)
{
  int error;
  FT_Library library;
  FT_Face face;
  FT_Matrix transform;
  FT_Glyph glyph;
  FT_BitmapGlyph bitmap_glyph;
  FT_Bitmap bitmap;
  unsigned long i, j, p;

  char *fontfn;
  unsigned long charcode;
  unsigned long size;
  unsigned long dpi;
  unsigned long stretch;
  char *outputfn;
  char outputfn_grey[256];
  char outputfn_mono[256];
  FILE *output;

  fontfn = argv[1];
  sscanf(argv[2], "%lu", &charcode);
  sscanf(argv[3], "%lu", &size);
  sscanf(argv[4], "%lu", &dpi);
  sscanf(argv[5], "%lu", &stretch);
  outputfn = argv[6];

  printf("Testing font '%s'\n", fontfn);
  printf("Charcode %lu\n", charcode);
  printf("Font size %lu\n", size);
  printf("DPI %lu\n", dpi);
  printf("Stretch %lu\n", stretch);
  printf("Output file '%s'\n", outputfn);

  if (error = FT_Init_FreeType(&library)) {
    printf("FT_Init_FreeType error: 0x%x\n", error);
    return 1;
  }

  {
    FT_Int major, minor, patch;
    FT_Library_Version(library, &major, &minor, &patch);
    printf("Freetype library version %d.%d.%d\n",
           major, minor, patch);
  }

  error = FT_New_Face(library, argv[1], 0, &face);
  if (error) {
    printf("FT_New_Face error: 0x%x\n", error);
    return 1;
  }

  if (stretch > 1) {
    transform.xx = 65536 / stretch;
    transform.xy = 0;
    transform.yx = 0;
    transform.yy = 65536;
    FT_Set_Transform(face, &transform, 0);
    if (error = FT_Set_Char_Size(face, (long)(size * 64), 0, dpi * stretch, dpi)) {
      printf("FT_Set_Char_Size error: 0x%x\n", error);
      return 1;
    }
  } else {
    if (error = FT_Set_Char_Size(face, size * 64, 0, dpi, dpi)) {
      printf("FT_Set_Char_Size error: 0x%x\n", error);
      return 1;
    }
  }

  /* Output greyscale */

  if (error = FT_Load_Char(face, charcode, FT_LOAD_NO_HINTING)) {
    printf("FT_Load_Char error: 0x%x\n", error);
    return 1;
  }

  if (error = FT_Get_Glyph(face->glyph, &glyph)) {
    printf("FT_Get_Glyph error: 0x%x\n", error);
    return 1;
  }

  if (error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1)) {
    printf("FT_Glyph_To_Bitmap error: 0x%x\n", error);
    return 1;
  }

  bitmap_glyph = (FT_BitmapGlyph)glyph;
  bitmap = bitmap_glyph->bitmap;

  strcpy(outputfn_grey, outputfn);
  strcat(outputfn_grey, "_grey.pgm");

  output = fopen(outputfn_grey, "w");
  fprintf(output, "P2\n%d %d\n256\n\n", bitmap.width, bitmap.rows);

  for (i = 0; i < bitmap.rows; ++i) {
    for (j = 0; j < bitmap.width; ++j) {
      fprintf(output, "%03hu ", 255 - bitmap.buffer[i * bitmap.pitch + j]);
    }
    fprintf(output, "\n");
  }

  fclose(output);

  /* Output monochrome */

  if (error = FT_Load_Char(face, charcode, FT_LOAD_NO_HINTING)) {
    printf("FT_Load_Char error: 0x%x\n", error);
    return 1;
  }

  if (error = FT_Get_Glyph(face->glyph, &glyph)) {
    printf("FT_Get_Glyph error: 0x%x\n", error);
    return 1;
  }

  if (error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, 0, 1)) {
    printf("FT_Glyph_To_Bitmap error: 0x%x\n", error);
    return 1;
  }

  bitmap_glyph = (FT_BitmapGlyph)glyph;
  bitmap = bitmap_glyph->bitmap;

  strcpy(outputfn_mono, outputfn);
  strcat(outputfn_mono, "_mono.pgm");

  output = fopen(outputfn_mono, "w");
  fprintf(output, "P2\n%d %d\n1\n\n", bitmap.width, bitmap.rows);

  for (i = 0; i < bitmap.rows; ++i) {
    for (j = 0; j < bitmap.width; ++j) {
      p = bitmap.buffer[i * bitmap.pitch + (j >> 3)] & (1 << (7 - (j & 0x7)));
      fprintf(output, "%01hu ", p ? 0 : 1);
    }
    fprintf(output, "\n");
  }

  fclose(output);

  FT_Done_Glyph(glyph);

  return 0;
}
