#include <lvgl.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include "spiffsDriver.h"

static void* fs_open(lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode) 
{
    const char* flags = "";
    if(mode == LV_FS_MODE_WR) flags = "w";
    else if(mode == LV_FS_MODE_RD) flags = "r";
    else if(mode == LV_FS_MODE_WR | LV_FS_MODE_RD) flags = "r+";

    char fullPath[32];
    if(path[0] != '/') 
    {
        snprintf(fullPath, sizeof(fullPath), "/%s", path);
    } 
    else 
    {
        strncpy(fullPath, path, sizeof(fullPath)-1);
    }
    
    File f = SPIFFS.open(fullPath, flags);
    if(!f) return NULL;

    return (void*)(new File(f));
}

static lv_fs_res_t fs_close(lv_fs_drv_t* drv, void* file_p) 
{
    File* fp = (File*)file_p;
    if(fp == NULL) return LV_FS_RES_INV_PARAM;
    
    fp->close();
    delete fp;
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br) 
{
    File* fp = (File*)file_p;
    if(fp == NULL) return LV_FS_RES_INV_PARAM;
    
    *br = fp->read((uint8_t*)buf, btr);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t* drv, void* file_p, uint32_t pos, lv_fs_whence_t whence) 
{
    File* fp = (File*)file_p;
    if(fp == NULL) return LV_FS_RES_INV_PARAM;
    
    SeekMode mode;
    if(whence == LV_FS_SEEK_SET) mode = SeekSet;
    else if(whence == LV_FS_SEEK_CUR) mode = SeekCur;
    else if(whence == LV_FS_SEEK_END) mode = SeekEnd;
    else return LV_FS_RES_INV_PARAM;

    fp->seek(pos, mode);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p) 
{
    File* fp = (File*)file_p;
    if(fp == NULL) return LV_FS_RES_INV_PARAM;
    
    *pos_p = fp->position();
    return LV_FS_RES_OK;
}

void initSpiffsDriver() 
{
    // Initialize SPIFFS
    if(!SPIFFS.begin(true)) 
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    Serial.println("SPIFFS Mounted Successfully");

    // Register SPIFFS driver for LVGL
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    fs_drv.letter = 'S';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    lv_fs_drv_register(&fs_drv);
    Serial.println("SPIFFS driver registered with LVGL");

    // Test if we can open a file through the driver
    if(SPIFFS.exists("/Logos.png")) {
        Serial.println("Testing SPIFFS driver with Logos.png");
        File f = SPIFFS.open("/Logos.png", "r");
        if(f) {
            Serial.println("Direct SPIFFS access works");
            f.close();
        }
    }
}