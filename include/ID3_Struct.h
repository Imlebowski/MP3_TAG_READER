/*
	ID3 v2.3 Structures
*/
#include <stdio.h>


#ifndef ID3_FRAMES
#define ID3_FRAMES
/* ID3 header format */
typedef struct
{
  char id[3];
  char ver[2];
  char flag;
  char size[4];
} ID3F;

/* ID3 Info */
typedef struct
{
  FILE *fptr;
  ID3F *id3f;
  int ver:16;
  uint size;
  void *TAGptr;
  uint Fpos;
} ID3Info;

/* ID3 v2.3 TAG header format */
typedef struct
{
  char id[4];
  char size[4];
  char flag[2];
} ID3_23S;

/* ID3 v2.3 TAG Info*/
typedef struct
{
  char id[4];
  uint size;
  uint pos;
  int tagId;
} ID3_23FInfo;

/* ID3 v2.3 APIC TAG Info */
typedef struct
{
  char *type;
  char *fname;
  uint pos;
  uint size;
} ID3_23_APIC;

/* ID3 Disp Data ptrs */
typedef struct
{
  char *fName;
  char *fsize;
  char *ver;
  char *title;
  char *artist;
  char *album;
  char *year;
  char *track;
  char *genre;
  char *lang;
  char *cmt;
} ID3_Disp;

#endif
