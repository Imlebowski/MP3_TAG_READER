/*
	functions to display or modify mp3 files
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ID3_Struct.h>
#include <types.h>
#include <ID3.h>

/* get operation type
 * input: commandline args
 * output: operation type
 */
Operation getOperationType(int argc, char *argv[])
{

  if (argc < 2)
  {
	printf("No arguments presnts\n");
	printf("use mp3tag -h for more info\n");
	return e_unsupported;
  }
  if (*argv[1] == '-')
  {
	switch (*(argv[1] + 1))
	{
	  case 't':
	  case 'T':
	  case 'a':
	  case 'A':
	  case 'y':
	  case 'c':
	  case 'g':
		if (argc != 5)
		{
	      printf("invalid format\n");
	      printf("use mp3tag -h for more info\n");
		  return e_unsupported;
		}
		return e_mod;
	  case 'i':
		if (argc != 3)
		{
	      printf("invalid format\n");
	      printf("use mp3tag -h for more info\n");
		  return e_unsupported;
		}
		return e_extract;
	  case 'I':
		if (argc != 5)
		{
	      printf("invalid format\n");
	      printf("use mp3tag -h for more info\n");
		  return e_unsupported;
		}
		return e_mImage;
	  case '-':
		if (strcmp(argv[1], "--help"))
		{
	      printf("invalid args\n");
	      printf("use mp3tag -h for more info\n");
		  return e_unsupported;
		}
	  case 'h':
		printf("%s\n", msg);
		return e_unsupported;
	  default:
	    printf("invalid args\n");
	    printf("use mp3tag -h for more info\n");
		return e_unsupported;
	}
  }
  else if (argc == 2)
  {
	return e_disp;
  }
  printf("invalid format\n");
  printf("use mp3tag -h for more info\n");
  return e_unsupported;
}

/* check ID3 header 
 * input: file info
 * output: ID3 info
 */
StatusInfo checkID3File(ID3Info *id3Info)
{
  StatusInfo SI;
  int idx;
  id3Info->id3f = (ID3F *)malloc(sizeof (ID3F));

  fread(id3Info->id3f, sizeof (ID3F), 1, id3Info->fptr);

  if (strncmp(id3Info->id3f->id, "ID3", 3))
  {
	SI.msg = "ID3 tag not found";
	SI.status = e_failure;
	return SI;
  }

  id3Info->ver = 0x0200 + id3Info->id3f->ver[0];
  id3Info->size = 0;

  for (idx = 0; idx < 4; idx++)
  {
	id3Info->size = (id3Info->size << 7) + id3Info->id3f->size[idx]; 
  }
	SI.status = e_success;
	return SI;
}

/* get size from str
 * input: str containing size
 * output: size
 */
uint getRevSize(char *ptr)
{
  uint size, idx;
  char *ptr1 = (char *)&size;

  for (idx = 0; idx < 4; idx++)
  {
	*ptr1++ = *(ptr + 3 - idx);  
  }
  return size;
}

/* check tag 
 * input: tag name
 * output: tag idx
 */
int checkTag_v23(char *ptr)
{
  int idx;
  for (idx = 0; idx < 8; idx++)
  {
	if (strncmp(ptr, ID3v23[idx], 4) == 0)
	{
      return idx;
	}
  }
	return -1;
}

/* get next tag info 
 * input: file info
 * output: tag info
 */
StatusInfo getNextTag_v23(ID3Info *id3Info)
{
  StatusInfo SI;
  ID3_23S id3_23s;
  ID3_23FInfo *id3_23finfo = (ID3_23FInfo *)malloc(sizeof (ID3_23FInfo));
  if (id3Info->TAGptr == NULL)
  {
	id3Info->Fpos = 10;
	fseek(id3Info->fptr, id3Info->Fpos, SEEK_SET);
  }
  else
  {
	id3Info->Fpos += ((ID3_23FInfo *)id3Info->TAGptr)->size + 10;
	fseek(id3Info->fptr, id3Info->Fpos, SEEK_SET);
    free(id3Info->TAGptr);
  }

  id3Info->TAGptr = (void *)id3_23finfo;


  if (id3Info->Fpos > (id3Info->size + 10))
  {
	printf("no\n");
    id3Info->TAGptr = NULL;
	SI.msg = "End of ID3 Tag header";
	SI.status = e_failure;
	return SI;
  }

  fread(&id3_23s, sizeof (ID3_23S), 1, id3Info->fptr);
  if (*id3_23s.id == 0)
  {
    id3Info->TAGptr = NULL;
	SI.msg = "Tag Not found";
	SI.status = e_failure;
	return SI;
  }

  if ((id3_23finfo->tagId = checkTag_v23(id3_23s.id)) == -1)
  {
	SI.msg = "Invalid Tag";
	SI.status = e_failure;
	return SI;
  }

  strncpy(id3_23finfo->id, id3_23s.id, 4);
  id3_23finfo->pos = id3Info->Fpos + 10;
  id3_23finfo->size = getRevSize(id3_23s.size);
  SI.status = e_success;
  return SI;
}

/* get file size 
 * input: file
 * output: file size
 */
uint getFileSize(FILE *fptr)
{
  uint size;
  uint cur_pos = ftell(fptr);
  fseek(fptr, 0, SEEK_END);
  size = ftell(fptr);
  fseek(fptr, cur_pos, SEEK_SET);
  return size;
}

/* file size to str 
 * input: file size
 * output: string
 */
char *fileSize2Str(uint size)
{
  char *buff = malloc(12 * sizeof (char));
  sprintf(buff, "%.2f KB", size / 1024.0);
  return buff;
}

/* copy n bytes to file 
 * input: data, len, file info
 * output: -
 */
void copyNBytes(FILE *dFptr, FILE *sFptr, uint len)
{
  char buff;
  uint tmp = ftell(sFptr);
  while ( !feof(sFptr) && len)
  {
	len--;
	buff = fgetc(sFptr);
	fputc(buff, dFptr);
  }
}

/* modify tag info 
 * input: tag info, file info
 * output: modified mp3 file
 */
StatusInfo ModifyTag(ID3Info *id3Info, int tagId, char *newStr, uint lenStr, FILE *Ofptr)
{
  StatusInfo SI;
  ID3_23FInfo *id3_23;
  char buff[4];
  ID3_23S frame;
  int diff, idx;
  fseek(id3Info->fptr, 0, SEEK_SET);
  while (1)
  {
    SI = getNextTag_v23(id3Info);
    if (SI.status == e_success)
	{
      id3_23 = (ID3_23FInfo *)id3Info->TAGptr;
	  if (id3_23->tagId == tagId)
	  {
		strncpy(frame.id, id3_23->id, 4);
		*(short *)frame.flag = 0;
		*(uint *)frame.size = lenStr + 1;
		*(uint *)frame.size = getRevSize(frame.size);

		fseek(id3Info->fptr, 0, SEEK_SET);
		copyNBytes(Ofptr, id3Info->fptr, id3Info->Fpos);
		fwrite(&frame, sizeof (ID3_23S), 1, Ofptr);
		fputc(0, Ofptr);
		fwrite(newStr, 1, lenStr , Ofptr);
		fseek(id3Info->fptr, id3_23->size + id3_23->pos, SEEK_SET);
		copyNBytes(Ofptr, id3Info->fptr, ~0);
		diff = lenStr  + 1 - id3_23->size;
		id3Info->size += diff;
		fseek(Ofptr, 6, SEEK_SET);
		*(uint *)buff = id3Info->size;
		uint temp = 0;
		for (idx = 0; idx < 4; idx++)
		{
		  temp <<= 8;
		  temp += (*(uint *)buff >> (7 * idx)) & 0x7f;
		}
		*(uint *)buff = temp;
		fwrite(buff, 1 , 4, Ofptr);
		fseek(Ofptr, 0, SEEK_END);
        SI.status = e_success;
  		return SI;
	  }
	}
	else if (id3Info->TAGptr == NULL)
	{
	  SI.msg = "Tag Not found";
	  SI.status = e_failure;
 	  return SI;
	}
  }
}


/* get single tag data 
 * input: tag info
 * output: tag data
 */
void getTagData_v23(ID3_Disp *id3disp, FILE *fptr, ID3_23FInfo *id3_23)
{
  char *buff;
  if (id3_23->tagId < 7 && id3_23->tagId >= 0)
  {
      buff = (char *)malloc(id3_23->size);
	  fseek(fptr, id3_23->pos + 1, SEEK_SET);
	  fread(buff, 1, id3_23->size - 1, fptr);
      buff[id3_23->size - 1] = '\0'; 
  }

  switch (id3_23->tagId)
  {
	case 0:
	  id3disp->title = buff;
	  return;
	case 1:
	  id3disp->artist = buff;
	  return;
	case 2:
	  id3disp->album = buff;
	  return;
	case 3:
	  id3disp->year = buff;
	  return;
	case 4:
	  id3disp->track = buff;
	  return;
	case 5:
	  id3disp->genre = buff;
	  return;
	case 6:
	  id3disp->lang = buff;
	  id3disp->cmt = buff + 4;
	  return;
	default:
	  return;
  }
}



/* prepare tag info 
 * input: file info
 * output: tag info 
 */
void prepareTagInfo_v23(ID3Info *id3Info, ID3_Disp *id3disp)
{
  StatusInfo SI; 
  ID3_23FInfo *id3_23;
  id3disp->ver = "v2.3";
  id3disp->title = "-";
  id3disp->artist = "-";
  id3disp->album = "-";
  id3disp->year = "XXXX";
  id3disp->track = "-";
  id3disp->genre = "-";
  id3disp->lang = "XXX";
  id3disp->cmt = "-";

  while (1)
  {

    SI = getNextTag_v23(id3Info);
    if (SI.status == e_success)
	{
      id3_23 = (ID3_23FInfo *)id3Info->TAGptr;
	  getTagData_v23(id3disp, id3Info->fptr, id3_23);
	}
	else if (id3Info->TAGptr == NULL)
	{
	  return;
	}
  }
}

/* disp tag info 
 * input: tag info, file info
 * output: -
 */
void dispTagInfo(ID3_Disp *id3disp)
{
  printf("%s\t\t[ %s ]\n", id3disp->fName, id3disp->fsize);
  printf("ID3 %s:\n", id3disp->ver);
  printf("title: %s\t\tartist: %s\n", id3disp->title, id3disp->artist);
  printf("album: %s\t\tyear: %s\n", id3disp->album, id3disp->year);
  printf("track: %s\t\tgenre: %s\n", id3disp->track, id3disp->genre);
  printf("comment: [lang: %s]\n%s\n", id3disp->lang, id3disp->cmt);
}

/* read line from file
 * input: file info
 * output: string
 */
void getStrFile(FILE *fptr, char *buff)
{
  do
  {
	*buff++ = fgetc(fptr);
  } while (*(buff - 1) != '\0');
}

/* get image tag info 
 * input: file info
 * output: image tag info
 */
void getAPIC(ID3Info *id3Info, ID3_23_APIC *id3A)
{
  char *buff = malloc(100);
  StatusInfo SI;
  ID3_23FInfo *id3_23;
  uint temp;
  ID3_23S frame;
  while (1)
  {
    SI = getNextTag_v23(id3Info);
    if (SI.status == e_success)
	{
      id3_23 = (ID3_23FInfo *)id3Info->TAGptr;
	  if (id3_23->tagId == 7)
	  {
		fseek(id3Info->fptr, id3_23->pos + 1, SEEK_SET);
		getStrFile(id3Info->fptr, buff);
		temp = strlen(buff);
		id3A->type = (char *)malloc(temp + 1);
		strcpy(id3A->type, buff);
	    fgetc(id3Info->fptr);
		getStrFile(id3Info->fptr, buff);
		temp = strlen(buff);
		id3A->fname = (char *)malloc(temp + 1);
		strcpy(id3A->fname, buff);
		id3A->pos = ftell(id3Info->fptr);
		id3A->size = id3_23->size - id3_23->pos + id3A->pos;
		return;
	  }
	}
	else if (id3Info->TAGptr == NULL)
	{
	  id3A = NULL;
 	  return;
	}
  }
  return;
}

/* extract image from file
 * input: image tag info, file info
 * output: image file
 */
StatusInfo extractImage(ID3Info *id3Info, ID3_23_APIC *id3A)
{
  StatusInfo SI;
  FILE *ifptr = fopen(id3A->fname, "w");
  if (ifptr == NULL)
  {
      printf("File : %s\n", id3A->fname);
	  SI.msg = "File cannot be opened";
	  SI.status = e_failure;
 	  return SI;
  }
  fseek(id3Info->fptr, id3A->pos, SEEK_SET);
  copyNBytes(ifptr, id3Info->fptr, id3A->size);
  fclose(ifptr);
  printf("Image created : %s\n", id3A->fname);
  SI.status = e_success;
  return SI;
}

/* modify image of mp3 file
 * input: image data, file info
 * output: modified mp3 file
 */
StatusInfo modiFyImage(ID3Info *id3Info, ID3_23_APIC *id3A, FILE *Ofptr) 
{
  StatusInfo SI;
  FILE *ifptr = fopen(id3A->fname, "r");
  if (ifptr == NULL)
  {
      printf("File : %s\n", id3A->fname);
	  SI.msg = "File cannot be opened";
	  SI.status = e_failure;
 	  return SI;
  }
  uint isize = getFileSize(ifptr);
  uint fnsize = strlen(id3A->fname) + 1;
  uint tysize = strlen(id3A->type) + 2;
  char *buff = malloc(isize + fnsize + tysize);
  strcpy(buff, id3A->type);
  buff[tysize - 2] = 0;
  buff[tysize - 1] = 0;
  strcpy(buff + tysize, id3A->fname);
  buff[tysize + fnsize - 1] = 0;
  fread(buff + tysize + fnsize, 1, isize, ifptr);
  SI = ModifyTag(id3Info, 7, buff, isize + fnsize + tysize, Ofptr);
  if (SI.status == e_failure)
  {
	  SI.status = e_failure;
 	  return SI;
  }
  fclose(ifptr);
  SI.status = e_success;
  return SI;
}
