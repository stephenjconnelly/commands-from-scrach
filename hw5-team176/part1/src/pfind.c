/* Author: Stephen Connelly, Benjamin Alexander 
 * ID: Sjc2235, Baa2165
 * On success, pfind will print the files that match the given permission string starting in the given directory
 *
 *
 *
 *
 */

//string = optarg 
//colon after flags in optarg part
//

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>


int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
S_IRGRP, S_IWGRP, S_IXGRP,
S_IROTH, S_IWOTH, S_IXOTH}; 

int checkperm(char* pstring){
    if(strlen(pstring)!= 9){
        return 0;
    }
    for(int i = 0; i < 9; i++){

        if(pstring[i] == '-'){
            continue;
        }else if(i%3 == 0){
            if(pstring[i] != 'r'){
                return 0; 
            } 
        }else if(i%3 == 1){
            if(pstring[i] != 'w'){
                return 0;
            }
        }else if(i%3 == 2){
            if(pstring[i] != 'x'){
                return 0;
            } 
        }
    }
    return 1;
}

char* permission_string(struct stat *statbuf){
    char *perm_string;
    if ((perm_string = malloc(10 * sizeof(char))) == NULL) {
        fprintf(stderr, "Error: malloc failed. %s.\n",
        	strerror(errno));
	exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < 9; i += 3) {
	// Using the ternary operator for succinct code.
	perm_string[i] = statbuf->st_mode & perms[i] ? 'r' : '-';
	perm_string[i + 1] = statbuf->st_mode & perms[i + 1] ? 'w' : '-';
	perm_string[i + 2] = statbuf->st_mode & perms[i + 2] ? 'x' : '-';
    }
    perm_string[9] = '\0';
   // printf("\n%s\n", perm_string);
   return perm_string;
}


int pfindhelp(char* dstring, char* pstring, char* path){
    DIR *dir;
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n",
            path, strerror(errno));
            return EXIT_FAILURE;
    }
    
    size_t pathlen = strlen(path);

    if(strcmp(path, "/")){
        path[pathlen++] = '/';
	path[pathlen]='0';
    }

    struct dirent *entry;
    struct stat sb;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
		strcmp(entry->d_name, "..") == 0) {
	    continue;
        }
    // Add the current entry's name to the end of full_filename, following
    // the trailing '/' and overwriting the '\0'.
        strncpy(path + pathlen, entry->d_name, PATH_MAX - pathlen);
        if (lstat(path, &sb) < 0) {
            fprintf(stderr, "Error: Cannot stat file '%s'. %s.\n",
                path, strerror(errno));
            continue;
        }
    // Differentiate directories from other file types.
        if (S_ISDIR(sb.st_mode)) {
        //recurse here
            pfindhelp(dstring, pstring, path);
	} else {
	  //  printf("%s\n", permission_string(&sb));
	   //  printf("%s\n", pstring);
	    if(strcmp(permission_string(&sb), pstring)==0){
               printf("%s\n", path);
            }
 	  //  printf("%s\n", path);
	}    
    }
    closedir(dir);
    return EXIT_SUCCESS;
}




int main(int argc, char **argv){
    int pflag = 0;
    int dflag = 0;
    int hflag = 0;
    int max_parameters = 5;
    int c;

    char* pstring;
    char* dstring;

    opterr = 0;

    while ((c = getopt(argc, argv, "d:p:h")) != -1) {
        switch (c)
            {
                case 'p':
                    pflag = 1;
		            pstring = optarg;
                    break;
                case 'd':
                    dstring = optarg;
		            dflag = 1;
                    break;
		case 'h':
		    hflag = 1;
		    break;
		case '?':  
		    fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
		    return EXIT_FAILURE;
            }
    }
    if (argc > max_parameters) {
        fprintf(stderr, "Too many arguments.\n");
        return EXIT_FAILURE;
    } else if(hflag){ 
	 fprintf(stdout, "Usage: pfind -d <directory> -p <permissions string> [-h]\n");
	 return EXIT_SUCCESS;
    } else if (!pflag){
        fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
	return EXIT_FAILURE;
    } else if (!dflag){
        fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
	return EXIT_FAILURE;
    }
    
    int isPerm = checkperm(pstring);
    if (!isPerm) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", pstring);
        return EXIT_FAILURE;
    }
    
    char path[PATH_MAX];
    if (realpath(dstring, path) == NULL) {
        fprintf(stderr, "Error: Cannot get full path of file '%s'. %s.\n",
            dstring, strerror(errno));
            return EXIT_FAILURE;
    }
    //pfindhelp will return exit status
    return pfindhelp(dstring, pstring, path);

}


