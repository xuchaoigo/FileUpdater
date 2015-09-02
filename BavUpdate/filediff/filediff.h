#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <openssl/md5.h>
#include <iostream>
#include <cstdio>
#include <iomanip>

#include "typedefs.h"
#include "../cJSON/cJSON.h"

#define OLD_VERSION 0
#define NEW_VERSION 1

#define PATH_MAX_LEN 128

class FileDiff
{
public:
	FileDiff();
  	~FileDiff();

  	VOID setOldPathPrefix(PCSTR dir);
  	VOID setNewPathPrefix(PCSTR dir);
  	VOID listDir(PCSTR dir, UINT depth,BOOL old_or_new); //recrusive
  	std::string diffFile();
  	std::string calcMD5(std::string full_path);

private:
  	std::set<std::string> old_dir;
  	std::set<std::string> new_dir;
  	std::string old_path_prefix;
  	std::string new_path_prefix;

};
