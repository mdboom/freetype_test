freetype_test: freetype_test.c
	gcc `freetype-config --cflags` freetype_test.c `freetype-config --libs` -o freetype_test
