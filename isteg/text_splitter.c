#include <stdio.h>

FILE *openforwrite(int filecounter)
{
	char fileoutputname[100];

	sprintf(fileoutputname, "text_files/file_part%d", filecounter);
	return fopen(fileoutputname, "w");
}

int main()
{
	FILE *ptr_readfile;
	FILE *ptr_writefile;
	char line[128]; /* or some other suitable maximum line size */
	int filecounter = 1, linecounter = 1;

	ptr_readfile = fopen("text_to_encode.txt", "r");
	if (!ptr_readfile)
		return 1;

	ptr_writefile = openforwrite(filecounter);

	while (fgets(line, sizeof line, ptr_readfile) != NULL)
	{
		if (linecounter == 24)
		{
			fclose(ptr_writefile);
			linecounter = 1;
			filecounter++;
			ptr_writefile = openforwrite(filecounter);
			if (!ptr_writefile)
				return 1;
		}
		fprintf(ptr_writefile, "%s\n", line);
		linecounter++;
	}
	fclose(ptr_readfile);
	return 0;
}