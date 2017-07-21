IFLAG := -I include/
SRCS := main.c
TARGET := MP3

#all : ${TARGET}
${TARGET} : ${SRCS}
	gcc $^ ${IFLAG} -o $@

clean :
	rm ${TARGET}
