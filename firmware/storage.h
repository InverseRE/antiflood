#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include "setting.h"

/* max number of storage records */
#define MAX_SETTIGS_NUMBER  20

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
        store_t str;     /* structured data */
        byte raw[44];    /* raw data        */
    } data;
    unsigned short crc;  /* check sum       */
} storage_t;

class SettingsStorage {

private:
    storage_t st;
    int pop(storage_t& st, bool load);
    int push(storage_t st);

public:
    SettingsStorage();
    
    int load();
    int create();
    bool exist();

    short enumerate(void);
    int read(short idx, Setting&);
    int update(short idx, Setting);
    int append(Setting);
    int erase_all(void);
};

#endif  /* __STORAGE_H__ */