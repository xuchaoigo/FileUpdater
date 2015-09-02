#include "filediff.h"


FileDiff::FileDiff()
{

}

FileDiff::~FileDiff()
{

}

VOID FileDiff::setOldPathPrefix(PCSTR dir)
{
	old_path_prefix = dir;
}
VOID FileDiff::setNewPathPrefix(PCSTR dir)
{
	new_path_prefix = dir;
}

INT FileDiff::listDir(PCSTR dir, UINT depth, BOOL old_or_new)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return BAV_ERROR;
    }
    if(chdir(dir) != 0) {
        fprintf(stderr,"cannot chdir directory: %s\n", dir);
        return BAV_ERROR;
    }
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;

            //printf("%*s%s/\n",depth,"",entry->d_name);//file is dir

            listDir(entry->d_name,depth+4,old_or_new);
        }
        else{
        	CHAR path_buf[PATH_MAX_LEN];
            if(getcwd(path_buf, sizeof(path_buf))==NULL)
            	return BAV_ERROR;
        	//DBG("%s/%s\n",path_buf,entry->d_name);
        	std::string strPath(path_buf);
        	strPath+="/";
        	strPath+=entry->d_name;
        	//cut of prefix

        	std::string this_prefix;
			if(old_or_new == OLD_VERSION)
				this_prefix = old_path_prefix;
			else
				this_prefix = new_path_prefix;
        	if(this_prefix=="")
        	{
        		ERR("ERROR:path_common_prefix is NULL!\n");
        		return BAV_ERROR;
        	}
        	UINT pos_prefix = strPath.find(this_prefix);
        	if(pos_prefix ==std::string::npos)
        	{
        		ERR("ERROR:invalid path = %s, prefix = %s\n",path_buf,this_prefix.c_str());
        		return BAV_ERROR;
        	}
        	//cout<<"full="<<strPath<<endl;
        	strPath = strPath.substr(pos_prefix+this_prefix.length());
        	//cout<<"sub="<<strPath<<endl;
        	if(old_or_new==OLD_VERSION)
        		old_dir.insert(strPath);
        	else
        		new_dir.insert(strPath);
        }
    }
    if(chdir("..") != 0) {
        fprintf(stderr,"cannot chdir directory: %s\n", "..");
        return BAV_ERROR;
    }
    closedir(dp);
    return BAV_OK;
}

std::string FileDiff::calcMD5(std::string full_path)
{

    FILE *fd=fopen(full_path.c_str(),"r");
    MD5_CTX c;
    UCHAR md5[17]={0};
    if(fd == NULL)
    {
		ERR("open failed\n");
		return "";
    }
    INT len;
    UCHAR *pData = (UCHAR*)malloc(64*1024*1024);
    if(!pData)
    {
		ERR("malloc failed\n");
        return "";
    }
    MD5_Init(&c);
    while( 0 != (len = fread(pData, 1, 64*1024*1024, fd) ) )
    {
            MD5_Update(&c, pData, len);
    }
    MD5_Final(md5,&c);

    CHAR  buf[33]={ '\0' };
    CHAR  tmp[3]={ '\0' };
    for(int i = 0; i < 16; i++)
    {
    	//printf("%x",md5[i]);
		sprintf(tmp, "%02x" ,md5[i]);
		strcat(buf,tmp);
    }

    std::string md5_str(buf);
    fclose(fd);
    free(pData);

    return md5_str;
}

std::string FileDiff::diffFile()
{
	std::set<std::string>::iterator it;
	/*
	cout<<"old_dir:"<<endl;
    for( it = old_dir.begin(); it != old_dir.end(); it++)
    	cout<<*it<<endl;
    cout<<"new_dir:"<<endl;
    for( it = new_dir.begin(); it != new_dir.end(); it++)
    	cout<<*it<<endl;
	 */
	std::set<std::string> add_set;
	std::set<std::string> del_set;
	std::set<std::string> patch_set;

    std::set_difference(new_dir.begin(), new_dir.end(),old_dir.begin(), old_dir.end(),\
    		std::inserter(add_set, add_set.begin()));
    std::set_difference(old_dir.begin(), old_dir.end(),new_dir.begin(), new_dir.end(),\
    		std::inserter(del_set, del_set.begin()));
    std::set_intersection(new_dir.begin(), new_dir.end(),old_dir.begin(), old_dir.end(),\
    		std::inserter(patch_set, patch_set.begin()));

    for( it = patch_set.begin(); it != patch_set.end(); )
    {
    	  std::string old_full_path = old_path_prefix+(*it);
    	  std::string new_full_path = new_path_prefix+(*it);
          if(0)//if(calcMD5(old_full_path)==calcMD5(new_full_path))
          {
        	  patch_set.erase(it++);
          }
          else
        	  it++;
    }
    /*
    std::cout<<"add_set:"<<std::endl;
    for( it = add_set.begin(); it != add_set.end(); it++)
    	std::cout<<*it<<std::endl;
    std::cout<<"del_set:"<<std::endl;
    for( it = del_set.begin(); it != del_set.end(); it++)
    	std::cout<<*it<<std::endl;
    std::cout<<"patch_set:"<<std::endl;
    for( it = patch_set.begin(); it != patch_set.end(); it++)
    	std::cout<<*it<<std::endl;
*/

    std::string diff_json;
    cJSON *proot,*plist,*pfile;
    CHAR* pout;

    proot=cJSON_CreateObject();

    cJSON_AddItemToObject(proot, "DownloadURL", cJSON_CreateString("172.17.180.69:/home/hips/karlxu/workspace/BavUpdateServer/version_repo"));
    cJSON_AddItemToObject(proot, "LatestVersion", cJSON_CreateString("ver1002"));

    //construct add list
    plist=cJSON_CreateArray();
    for(it = add_set.begin(); it != add_set.end(); it++)
	{
		cJSON_AddItemToArray(plist,pfile=cJSON_CreateObject());
		cJSON_AddStringToObject(pfile, "path",(*it).c_str());
		cJSON_AddStringToObject(pfile, "md5",calcMD5(new_path_prefix+*it).c_str());
	}
	cJSON_AddItemToObject(proot,"add_list", plist);

	//construct delete list
    plist=cJSON_CreateArray();
    for(it = del_set.begin(); it != del_set.end(); it++)
	{
		cJSON_AddItemToArray(plist,pfile=cJSON_CreateObject());
		cJSON_AddStringToObject(pfile, "path",(*it).c_str());
		cJSON_AddStringToObject(pfile, "md5","");
	}
	cJSON_AddItemToObject(proot,"del_list", plist);

	//construct patch list
    plist=cJSON_CreateArray();
    for(it = patch_set.begin(); it != patch_set.end(); it++)
	{
		cJSON_AddItemToArray(plist,pfile=cJSON_CreateObject());
		cJSON_AddStringToObject(pfile, "path",(*it).c_str());
		cJSON_AddStringToObject(pfile, "md5",calcMD5(new_path_prefix+*it).c_str());
	}
	cJSON_AddItemToObject(proot,"patch_list", plist);
	pout=cJSON_Print(proot);
	diff_json = pout;
	cJSON_Delete(proot);
	free(pout);

	DBG("diff_json = \n%s\n",diff_json.c_str());
    return diff_json;
}


