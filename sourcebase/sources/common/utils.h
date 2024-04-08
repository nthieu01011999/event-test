#ifndef __UTILS__
#define __UTILS__

#include "app.h"
#include "../common/json.hpp"

using json = nlohmann::json;

extern string get_mac_address();
extern string get_device_id();

extern bool write_raw_file(const string data, const string &filename);
extern bool write_json_file(const json data, const string &filename);

extern bool read_raw_file(string &data, const string &filename);
extern bool read_json_file(json &data, const string &filename);

extern void erase_all_string(string &main_string, string &erase_string);
extern void replace_all_string(string &main_string, string &find_string, string &repl_string);

extern void bytetoHexChar(uint8_t ubyte, uint8_t *uHexChar);
extern void bytestoHexChars(uint8_t *ubyte, int32_t len, uint8_t *uHexChar);
extern void hexChartoByte(uint8_t *uHexChar, uint8_t *ubyte);
extern void hexCharsToBytes(uint8_t *uHexChar, int32_t len, uint8_t *ubyte);

extern string get_ip_address();
extern int get_list_music(const char *path, json &list);
extern uint32_t get_usb_usage(const char *path);
extern uint32_t get_usb_remain(const char *path);
extern bool check_usb_encrypted(const char *script);
extern int touch_full_file(const char *file_name);
extern bool check_file_exist(const char *file_name);

extern int exec_prog(int, const char **);

extern vector<string> string_split(string &s, string delimiter);
extern size_t sizeOfFile(const char *url);

extern int systemCmd(const char *fmt, ...);
extern char *safe_strcpy(char *dest, const char *src, size_t dest_size);
extern string getFileName(const string &s);
extern bool appendStringToFile(const char *path, const string str);

template<class T>
std::weak_ptr<T> make_weak_ptr(std::shared_ptr<T> ptr) {
	return ptr;
}

#endif	  //__UTILS__
