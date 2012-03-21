#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <stdio.h>

#define VERTICAL_HINTING
#define HORIZ_HINTING 8

int main(int argc, char** argv)
{
  int error;
  FT_Library library;
  FT_Face face;
  FT_Matrix transform = { 65536 / HORIZ_HINTING, 0, 0, 65536 };
  FT_ULong charcode;
  FT_Glyph glyph;
  FT_BBox bbox;

  charcode = sscanf(argv[2], "%lu", &charcode);

  printf("Testing font '%s'\n", argv[1]);
  printf("Testing charcode '%lu'\n", charcode);

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

 #ifdef VERTICAL_HINTING
  FT_Set_Transform(face, &transform, 0);

  if (error = FT_Set_Char_Size(face, 12 * 64, 0, 72 * HORIZ_HINTING, 72)) {
    printf("FT_Set_Char_Size error: 0x%x\n", error);
    return 1;
  }
 #else
  if (error = FT_Set_Char_Size(face, 12 * 64, 0, 72, 72)) {
    printf("FT_Set_Char_Size error: 0x%x\n", error);
    return 1;
  }
 #endif

  if (error = FT_Load_Char(face, charcode, FT_LOAD_NO_HINTING)) {
    printf("FT_Load_Char error: 0x%x\n", error);
    return 1;
  }

  if (error = FT_Get_Glyph(face->glyph, &glyph)) {
    printf("FT_Get_Glyph error: 0x%x\n", error);
    return 1;
  }

  FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_subpixels, &bbox);

  printf("CBox: xMin: %ld yMin: %ld xMax: %ld yMax: %ld\n",
         bbox.xMin, bbox.yMin, bbox.xMax, bbox.yMax);

  return 0;
}
