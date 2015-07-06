#include "filelisting_RP.hpp"

//Natural sort the filenames

int strcompare(const char *fname1, const char *fname2){
	for(;;){
		if(*fname2 == '\0'){
			return *fname1 != '\0';}
		else if(*fname1 == '\0'){
			return -1;}
        else if(!(isdigit(*fname1) && isdigit(*fname2))){
			if (*fname1 != * fname2){
			return (int)*fname1 - (int)*fname2;}
			else {(++fname1,++fname2);}
		}else {
			char *limit1, *limit2;
			unsigned long n1 = strtoul(fname1,&limit1,10);
			unsigned long n2 = strtoul(fname2,&limit2,10);
			if (n1 > n2){
				return 1;
			}
			else if(n1<n2){
				return -1;
			}
			fname1 = limit1;
			fname2 = limit2;
		}
	}
}

static int qsort_natural(const void *s1, const void *s2){
    const char* const *compare1 = static_cast<const char * const *>(s1);
	const char* const *compare2 = static_cast<const char * const *>(s2);
	return strcompare(*compare1,*compare2);
}

void freefilenames(filenames *ff){
	free(ff->names);
	ff->names=NULL;
	ff->used = ff->size = 0;
}
void initNames(filenames *ff, int initsize){
	(ff->names) = (char **) malloc(initsize * sizeof(char*));
	ff->used=0;
	ff->size = initsize;
}
void insertNames(filenames *ff, char *name){
	if(ff->used == ff->size){
		ff->size *=2;
		ff->names=(char **)realloc(ff->names,ff->size * sizeof(char*));
	}
	ff->names[ff->used] = (char *)malloc(sizeof(char) * (strlen(name)+1));
	strcpy(ff->names[ff->used++],name);
}
filenames file_listing(char *pathin, char *filter, int echo){
	filenames ff;
	int i,ch,j,k;
	tinydir_dir dir;
    char *ext;
	if (tinydir_open_sorted(&dir, pathin) == -1)
	{
		perror("Error opening directory");
		if(strcmp(pathin,"")>0){
			tinydir_close(&dir);
		}
			ff.size=NULL;
			ff.used=NULL;
			return ff;
	}
	initNames(&ff,1);
	tinydir_file file;
	if(strcmp(filter,"\0")>0){
	for(i=0;i<dir.n_files;i++){
		if(tinydir_readfile_n(&dir,&file,i)!=-1 && !file.is_dir){
			k=0;
			ext = (char *)malloc(strlen(filter)*sizeof(char));
			for(j=strlen(file.path)-strlen(filter);j<=strlen(file.path);j++){
				ext[k++] = file.path[j];
			}
			if(strcmp(ext,filter)==0){
			insertNames(&ff,file.path);
			if(echo){
			printf("%s\n",ff.names[ff.used-1]);
			}
			}
		}
		
	} 
	if(echo){
	printf("The total number of files with the extension %s is :%d\n",filter,ff.used);
	}
	}else{
		for(i=0;i<dir.n_files;i++){
		if(tinydir_readfile_n(&dir,&file,i)!=-1 && !file.is_dir){
			insertNames(&ff,file.path);
			if(echo){
			printf("%s\n",ff.names[ff.used-1]);
			}
		}
		}
	if(echo){
	printf("The total number of files in the directory is :%d\n",ff.used);
	}
	}

	qsort(ff.names,ff.used,sizeof(ff.names),qsort_natural);
	return ff;

}
