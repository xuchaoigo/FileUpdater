#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "../cJSON/cJSON.h"
#include "typedefs.h"

class Updater
{
public:
	Updater();
  	~Updater();
  	INT doUpdate(std::string diff_str,PCSTR old_dir,PCSTR new_dir,PCSTR patch_dir);
  	INT parseDiffStr(std::string diff_str);
  	INT downloadPatch();
  	INT updateFiles();
private:
	static const INT LEN = 128;
	CHAR cmd[LEN];

	INT sysCall(PCSTR cmd);
  	std::string old_dir;
  	std::string new_dir;
  	std::string patch_dir;
	std::vector<std::string> add_list;
	std::vector<std::string> del_list;
	std::vector<std::string> patch_list;
};
