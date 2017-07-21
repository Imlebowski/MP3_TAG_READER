
#ifndef ID3_H
#define ID3_H
/* help menu */
 char *msg = \
"usage:	mp3tag file 							\n\
  	mp3tag -h									\n\
  	mp3tag -[etaAycgI] \"value\" file1 file2	\n\
  	mp3tag -i file								\n\
	-t 	Modifies a Title tag				   	\n\
	-T	Modifies a Track tag					\n\
	-a	Modifies an Artist tag					\n\
	-A	Modifies an Album tag					\n\
	-y	Modifies a Year tag						\n\
	-c	Modifies a Comment tag					\n\
	-g	Modifies a Genre tag					\n\
	-h	Displays this help info[--help]			\n\
	-I	Modifies Image 							\n\
	-i	Extracts Image 							\n";

/* required TAGS */
char *ID3v23[] = {"TIT2", "TPE1", "TALB", "TYER", "TRCK", "TCON", "COMM", "APIC"};

/* funtion prototype */
/* get operation type */
Operation getOperationType(int argc, char *argv[]);

/* check if file has ID3 */
StatusInfo checkID3File(ID3Info *id3Info);

/* get size */
uint getRevSize(char *ptr);

/* check tags */
int checkTag_v23(char *ptr);

/* get next tag */
StatusInfo getNextTag_v23(ID3Info *id3Info);

/* get size of file */
uint getFileSize(FILE *fptr);

/* get str of file size */
char *fileSize2Str(uint size);

/* move nbytes of data to file */
void copyNBytes(FILE *dFptr, FILE *sFptr, uint len);

/* modify tag */
StatusInfo ModifyTag(ID3Info *id3Info, int tagId, char *newStr, uint lenStr, FILE *Ofptr);

/* get tag data */
void getTagData_v23(ID3_Disp *id3disp, FILE *fptr, ID3_23FInfo *id3_23);

/* prepare disp info */
void prepareTagInfo_v23(ID3Info *id3Info, ID3_Disp *id3disp);

/* disp tag info */
void dispTagInfo(ID3_Disp *id3disp);

/* get string from file */
void getStrFile(FILE *fptr, char *buff);

/* get image info */
void getAPIC(ID3Info *id3Info, ID3_23_APIC *id3A);

/* extract image */
StatusInfo extractImage(ID3Info *id3Info, ID3_23_APIC *id3A);

/* modify image */
StatusInfo modiFyImage(ID3Info *id3Info, ID3_23_APIC *id3A, FILE *Ofptr);

#endif
