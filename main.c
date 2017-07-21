/*
	program to display and modify mp3 ID3v2.3 TAGS
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ID3_Struct.h>
#include <types.h>
#include <ID3.h>
#include "ID3.c"

/* Main function */
void main(int argc, char *argv[])
{
  StatusInfo SI;
  int tagId;
  char strbuff[100] = "";
  uint lenStr;
  FILE *Dptr;
  ID3_23_APIC *id3A;

  /* get operation type using command line args */
  Operation op = getOperationType(argc, argv);
  /* check if invalid type */
  if (op == e_unsupported)
  {
	return;
  }

  ID3Info *id3Info = (ID3Info *)malloc(sizeof (ID3Info));
  /* perform required opeartion */
  switch (op)
  {
	case e_disp:
	  /* disp mp3 tag info */
	  id3Info->fptr = fopen(argv[1], "r");
	  if (id3Info->fptr == NULL)
	  {
		printf("FIle %s not found\n", argv[1]);
		return;
	  }
  	  SI = checkID3File(id3Info);
  	  if (SI.status == e_failure)
  	  {
		printf("ID3 header not found\n");
		return;
  	  }
  	  ID3_Disp id3disp;
  	  id3disp.fName = argv[1];
  	  id3disp.fsize = fileSize2Str(getFileSize(id3Info->fptr));
  	  prepareTagInfo_v23(id3Info, &id3disp);
  	  dispTagInfo(&id3disp);
	  fclose(id3Info->fptr);
	  break;
	case e_mod:
	  /* modify required tag */
	  id3Info->fptr = fopen(argv[3], "r");
	  if (id3Info->fptr == NULL)
	  {
		printf("FIle %s not found\n", argv[3]);
		return;
	  }

	  Dptr = fopen(argv[4], "w");
	  if (Dptr == NULL)
	  {
		printf("FIle %s cannot be opened\n", argv[4]);
		return;
	  }
  	  SI = checkID3File(id3Info);
  	  if (SI.status == e_failure)
  	  {
		printf("ID3 header not found\n");
		return;
  	  }
	  switch (*(argv[1] + 1))
	  {
	    case 't':
		  tagId = 0;
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  break;
	    case 'T':
		  tagId = 4;
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  break;
	    case 'a':
		  tagId = 1;
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  break;
	    case 'A':
		  tagId = 2;
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  break;
	    case 'y':
		  tagId = 3;
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  if (lenStr != 4)
		  {
			printf("wrong format for year\n");
			return;
		  }
		  break;
	    case 'c':
		  tagId = 6;
		  strcpy(strbuff, "eng ");
		  strcat(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  strbuff[3] = '\0'; 
		  break;
	    case 'g':
		  strcpy(strbuff, argv[2]);
		  lenStr = strlen(strbuff);
		  tagId = 5;
		  break;
	  }
      	  SI = ModifyTag(id3Info, tagId, strbuff, lenStr, Dptr);
  	  if (SI.status == e_failure)
  	  {
		printf("%s\n", SI.msg);
		return;
  	  }
	  printf("Tag moddified\n");
	  fclose(id3Info->fptr);
	  fclose(Dptr);
	  break;
	case e_extract:
	  /* extract image */
	  id3Info->fptr = fopen(argv[2], "r");
	  if (id3Info->fptr == NULL)
	  {
		printf("FIle %s not found\n", argv[2]);
		return;
	  }
  	  SI = checkID3File(id3Info);
  	  if (SI.status == e_failure)
  	  {
		printf("ID3 header not found\n");
		return;
  	  }
	  id3A = (ID3_23_APIC *)malloc(sizeof (ID3_23_APIC));
	  getAPIC(id3Info, id3A);
	  if (id3A == NULL)
	  {
		printf("Image not found\n");
		return;
	  }
	  SI = extractImage(id3Info, id3A); 
  	  if (SI.status == e_failure)
  	  {
		printf("%s\n", SI.msg);
		return;
  	  }
	  fclose(id3Info->fptr);
	  break;
	case e_mImage:
	  /* modify image present in mp3 */
	  id3Info->fptr = fopen(argv[3], "r");
	  if (id3Info->fptr == NULL)
	  {
		printf("FIle %s not found\n", argv[3]);
		return;
	  }
	  Dptr = fopen(argv[4], "w");
	  if (Dptr == NULL)
	  {
		printf("FIle %s cannot be opened\n", argv[4]);
		return;
	  }
  	  SI = checkID3File(id3Info);
  	  if (SI.status == e_failure)
  	  {
		printf("ID3 header not found\n");
		return;
  	  }
	  id3A = (ID3_23_APIC *)malloc(sizeof (ID3_23_APIC));
	  id3A->type = "image/jpeg";
	  id3A->fname = argv[2];
	  SI = modiFyImage(id3Info, id3A, Dptr);
  	  if (SI.status == e_failure)
  	  {
		printf("%s\n", SI.msg);
		return;
  	  }
	  printf("Tag moddified\n");
	  fclose(id3Info->fptr);
	  fclose(Dptr);
	  break;
  }
  free(id3Info);
}
