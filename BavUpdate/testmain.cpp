#include "myhttp/MyHttp.h"
#include "cJSON/cJSON.h"
#include "filediff/filediff.h"
#include "updater/updater.h"

int main()
{
#if 0
	DBG("test main\n");
	PCSTR olddir = "ver1001";
	PCSTR newdir = "ver1002";
	FileDiff fd;
    printf("olddir = %s\n",olddir);
    printf("newdir = %s\n",newdir);
    fd.setOldPathPrefix(olddir);
    fd.setNewPathPrefix(newdir);
    fd.listDir(olddir,0,OLD_VERSION);
    fd.listDir(newdir,0,NEW_VERSION);
    std::string diff_str = fd.diffFile();
    return 0;
#endif

#if 1
	MyHttp h("172.17.180.69",8888);
	h.establishConnection();
	std::string diff_str = h.test_send_recv();
	h.destroyConnection();

#endif

    Updater ud;
	PCSTR olddir_update = "android/ver1001";
	PCSTR newdir_update = "android/ver1002";
	PCSTR patchdir_update = "android/ver1002_patch";
	INT ret = ud.doUpdate(diff_str,olddir_update,newdir_update,patchdir_update);
    if(ret == BAV_OK){
    	DBG("\nUpdate complete.\n");
    }
    else{
    	DBG("\nUpdate failed.\n");
    }
	return 0;
}
