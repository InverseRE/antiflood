#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include "debug.h"
#include "setting.h"

/* max number of storage records */
#define MAX_SETTIGS_NUMBER  10

/* storage raw data size */
#define RAW_SIZE  ((MAX_SETTIGS_NUMBER) * sizeof(short) + 4)

/** Error codes. */
enum StorageErrorCodes {
    ST_OK = 0,
    ST_READ_ERROR = 10,
    ST_WRITE_ERROR = 20,
    ST_BAD_ARGUMENT = 30,
    ST_FULL = 40
};

/* storage structure */
typedef struct store {
    short address[MAX_SETTIGS_NUMBER]; /* list of recs address */
    short number;                      /* actual recs number   */
    short next_free;                   /* next free address    */
} store_t;
    
typedef struct storage {
    union {
        store_t str;           /* structured data */
        byte raw[RAW_SIZE];    /* raw data        */
    } data;
    unsigned short crc;        /* check sum       */
} storage_t;
    
class SettingsStorage {

public:
    static SettingsStorage& get_instance() {
        static SettingsStorage s;
        return s;
    }
    
    int load();
    int create();
    bool exist();

    short enumerate(void);
    int read(short idx, Setting&);
    int update(short idx, Setting);
    int append(Setting);
    int erase_all(void);
    
    void eeprom_dp(void);
    
private:
    storage_t st;

    SettingsStorage() {}
    ~SettingsStorage() {}
    SettingsStorage(SettingsStorage const&) = delete;
    SettingsStorage& operator= (SettingsStorage const&) = delete;
    
    int pop(bool load);
    int push(void);


};

#endif  /* __STORAGE_H__ */