
#include "updater.h"


Updater::Updater()
{

}

Updater::~Updater()
{

}

INT Updater::parseDiffStr(std::string diff_str)
{
    cJSON *json ,*json_string, *json_array;
    std::string LatestVersion,DownloadURL;
    json = cJSON_Parse(diff_str.c_str());
    if (!json)
    {
        ERR("JSON Format err at: [%s]\n",cJSON_GetErrorPtr());
        return BAV_ERROR;
    }
    else
    {
    	//parse latest version
    	json_string = cJSON_GetObjectItem(json ,"LatestVersion");
        if(json_string->type == cJSON_String )
        {
        	LatestVersion = json_string->valuestring;
        }
        else
        {
        	ERR("no LatestVersion!\n");
        	return BAV_ERROR;
        }

        json_string = cJSON_GetObjectItem(json,"DownloadURL");
        if(json_string->type == cJSON_String)
        {
        	DownloadURL = json_string->valuestring;
        }
        else
        {
        	ERR("no DownloadURL!\n");
        	return BAV_ERROR;
        }
    	//parse add_list
        json_array = cJSON_GetObjectItem(json , "add_list");
        if( json_array->type == cJSON_Array )
        {
            //INT arrySize=cJSON_GetArraySize(json_array);
            cJSON *item=json_array->child;
            while(item!=NULL)
            {
                DBG("%s: %s\n",cJSON_GetObjectItem(item,"path")->valuestring, \
                		cJSON_GetObjectItem(item,"md5")->valuestring);
                add_list.push_back(cJSON_GetObjectItem(item,"path")->valuestring);
                item=item->next;
            }
        }
        else
        {
        	ERR("no add_list!\n");
        	return BAV_ERROR;
        }
    	//parse del_list
        json_array = cJSON_GetObjectItem(json , "del_list");
        if( json_array->type == cJSON_Array )
        {
            //INT arrySize=cJSON_GetArraySize(json_array);
            cJSON *item=json_array->child;
            while(item!=NULL)
            {
                DBG("%s: %s\n",cJSON_GetObjectItem(item,"path")->valuestring, \
                		cJSON_GetObjectItem(item,"md5")->valuestring);
                del_list.push_back(cJSON_GetObjectItem(item,"path")->valuestring);
                item=item->next;
            }
        }
        else
        {
        	ERR("no del_list!\n");
        	return BAV_ERROR;
        }
    	//parse patch_list
        json_array = cJSON_GetObjectItem(json , "patch_list");
        if( json_array->type == cJSON_Array )
        {
            //INT arrySize=cJSON_GetArraySize(json_array);
            cJSON *item=json_array->child;
            while(item!=NULL)
            {
                DBG("%s: %s\n",cJSON_GetObjectItem(item,"path")->valuestring, \
                		cJSON_GetObjectItem(item,"md5")->valuestring);
                patch_list.push_back(cJSON_GetObjectItem(item,"path")->valuestring);
                item=item->next;
            }
        }
        else
        {
        	ERR("no patch_list!\n");
        	return BAV_ERROR;
        }

        cJSON_Delete(json);
    }
    DBG("LatestVersion=%s\n",LatestVersion.c_str());
    DBG("DownloadURL=%s\n",DownloadURL.c_str());

    return BAV_OK;
}

INT Updater::downloadPatch()
{

	return BAV_OK;
}

INT Updater::sysCall(PCSTR cmd)
{
	DBG("%s\n",cmd);
	return system(cmd);
	//return 0;
}

INT Updater::updateFiles()
{
	if(old_dir==""||new_dir==""||patch_dir=="")
	{
		ERR("3dir is empty\n");
		return BAV_ERROR;
	}

	DBG("\n#DO WHOLE DIR COPY\n");
	memset(cmd,0,LEN);
	sprintf(cmd,"cp -rf %s %s",old_dir.c_str(),new_dir.c_str());
	if(BAV_OK!=sysCall(cmd))
		return BAV_ERROR;

	DBG("\n##DO DELETE\n");
	std::vector<std::string>::iterator it;
	for(it = del_list.begin(); it!=del_list.end();it++)
	{
		memset(cmd,0,LEN);
		sprintf(cmd,"rm -rf %s",(new_dir+(*it)).c_str());
		if(BAV_OK!=sysCall(cmd))
			return BAV_ERROR;
	}

	DBG("\n##DO ADD\n");
	for(it = add_list.begin(); it!=add_list.end();it++)
	{
		memset(cmd,0,LEN);
		std::string full_path_new_dir = new_dir+(*it);//the full path of add_file
		std::string folder_path_new_dir ;//the folder path of add_file
		size_t pos = full_path_new_dir.rfind("/");
		if(pos==std::string::npos)
			continue;
		if(full_path_new_dir.rfind("/",pos-1) != std::string::npos)
		{
			folder_path_new_dir = full_path_new_dir.substr(0,pos);
			memset(cmd,0,LEN);
			sprintf(cmd,"mkdir %s",folder_path_new_dir.c_str());
			sysCall(cmd);
			//mkdir 可能因为重复而失败.由copy是否成功来检验
		}
		memset(cmd,0,LEN);
		sprintf(cmd,"cp -rf %s %s",(patch_dir+(*it)).c_str(),(new_dir+(*it)).c_str());
		if(BAV_OK!=sysCall(cmd))
			return BAV_ERROR;
	}

	DBG("\n##DO PATCH\n");
	for(it = patch_list.begin(); it!=patch_list.end();it++)
	{
		//step1
		memset(cmd,0,LEN);
		sprintf(cmd,"rm -rf %s",(new_dir+(*it)).c_str());
		if(BAV_OK!=sysCall(cmd))
			return BAV_ERROR;

		//step2
		memset(cmd,0,LEN);
		std::string full_path_new_dir = new_dir+(*it);//the full path of add_file
		std::string folder_path_new_dir ;//the folder path of add_file
		size_t pos = full_path_new_dir.rfind("/");
		if(pos==std::string::npos)
			continue;
		if(full_path_new_dir.rfind("/",pos-1) != std::string::npos)
		{
			folder_path_new_dir = full_path_new_dir.substr(0,pos);
			memset(cmd,0,LEN);
			sprintf(cmd,"mkdir %s",folder_path_new_dir.c_str());
			sysCall(cmd);
			//mkdir 可能因为重复而失败.由copy是否成功来检验
		}
		memset(cmd,0,LEN);
		sprintf(cmd,"cp -rf %s %s",(patch_dir+(*it)).c_str(),(new_dir+(*it)).c_str());
		if(BAV_OK!=sysCall(cmd))
			return BAV_ERROR;
	}
	return BAV_OK;
}

INT Updater::doUpdate(std::string diff_str,PCSTR old_dir_str,PCSTR new_dir_str,PCSTR patch_dir_str)
{
	old_dir = old_dir_str;
	new_dir = new_dir_str;
	patch_dir = patch_dir_str;
	DBG("doUpdate!\n");
	if(BAV_OK != parseDiffStr(diff_str))
	{
		return BAV_ERROR;
	}
	if(BAV_OK != downloadPatch())
	{
		return BAV_ERROR;
	}
	if(BAV_OK != updateFiles())
	{
		return BAV_ERROR;
	}
	return BAV_OK;
}
