/*
 * IOUtil.c
 *
 *  Created on: Sep 11, 2018
 *      Author: WanQing
 */
#include <utils/io.h>
#include <cutils/strutils.h>

char *file_read(char *path) {
	FILE *fp = fopen(path, "r");
	if (!fp)
		return 0;
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *content = (char *) malloc((size + 1) * sizeof(char));
	int hasread = fread(content, sizeof(char), size, fp);
	if (hasread != size)
		content = realloc(content, (hasread + 1) * sizeof(char));
	content[hasread] = '\0';
	fclose(fp);
	return content;
}
qreader *reader_create(char *path) {
	qreader *reader = qmalloc(qreader);
	reader->file = fopen(path, "r");
	return reader;
}
int read_line(qreader *reader) {
	return fgets(reader->content, READ_SIZE, reader->file);
}
void reader_destroy(qreader *reader) {
	if (reader->file)
		fclose(reader->file);
	qfree(reader);
}
