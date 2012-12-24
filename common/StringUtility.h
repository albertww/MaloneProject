/*
**	Author: Albert
**	Date: 2012-04-17
**	Description:  some c-style-string and stl string utilities
*/

#ifndef __STRING_UTILITY_H__
#define __STRING_UTILITY_H__

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h> // wcstombs mbstowcs
#include <inttypes.h> // uint_x

using namespace std;


// remove spaces at head and tail of a string
char * Trim(char *ptr);
// you have an orgin string "a xxx b yyy", "a" and "b" are field, "xxx" "yyy" are their
// values, you can use this function to get the values of the field
// @param src, the src string
// @param field, the value of field want to read
// @param value, output, the value of field is returned
// @return, 1 suc, 0 failed
int GetField(char *src, char *field, char *value);
// split a string by seperator sep
// e.g  ",aaa,bbbb,cccc" => {, aaa, bbbb, cccc}
// caution, in front aaa is a seperator, so there is an empty element
int StrSplit(char *src, const char *sep, vector<string> &vec);
// split a string by spaces
// you can extract data regardless of spaces
// e.g " aaa  bbb cccc    ddddd  " => {aaa, bbb, cccc, ddddd} 
int StrSplitBySpaces(char *src, vector<string> &vec);
// join string to a single string with seperator
// e.g "aaa" "bbb" "ccc", you can define any seperator you want, such as "|"
// after invoke this function you will get "aaa|bbb|ccc"
int StrJoin(char *dst, char sep, vector<string> &vec);

string WStringToString(const wstring& wstr);
wstring StringToWString(const string& str);
string WStringToUTF8(const wstring& wstr);
wstring UTF8ToWString(const string& str);

string WChar2Ansi(wstring szWideStr);
wstring Ansi2WChar(string szAscStr);

// 把字符串的内容倒置，比如"abc" -> "cba"
void ReverseString(char *str);

// 把15以内的数转为16进制的表达式，比如10 -> 'A',  15 -> 'F'
// @return 成功返回'0' - '9' 'A' - 'F', 失败返回'W'表示wrong，失败的原因是因为数字大于15
char Digit2Hex(uint64_t val);

/*
1byte 2^8 = 255    				3
2byte 2^16 = 64,000    				5
3byte 2^24 = 16,000,000				8
4byte 2^32 = 4,000,000,000			10
5byte 2^40 = 1,000,000,000,000			13
6     2^48 = 255,000,000,000,000		15
7     2^56 = 64,000,000,000,000,000		17
8     2^64 = 16,000,000,000,000,000,000		20
每增加1byte，字符串表达式后面最多多3个0
n byte的十进制字符串表达式 小于等于 n * 3，为存放字符串末尾的NULL + 1

1byte 2^8 = FF    					2
2byte 2^16 = FFFF    				4
3byte 2^24 = FFFF FF				5
4byte 2^32 = FFFF FFFF				8
5byte 2^40 = FFFF FFFF FF			10
6     2^48 = FFFF FFFF FFFF 		12
7     2^56 = FFFF FFFF FFFF FF		14
8     2^64 = FFFF FFFF FFFF FFFF	16
每增加1byte，字符串表达式多两位
n byte的十六进制字符串表达式 小于等于 n * 2，为存放字符串末尾的NULL + 1
*/

// 把64位无符号整数转为其字符串表达式，比如 987 -> "987"
// 64位整型，最大为2^64 = 2^4 * 2^30 * 2^30 = 16 * 1G * 1G ~ 16,000,000,000,000,000,000，所以uint64结构转为10进制表达式
// 的最大长度为20位，再加上我们NULL来表示字符串结尾，任何64位无符号整数都可以转化为长度为21的字符串
// @param octString, 存放结果，需要保证octString的长度大于21
// You can think this as itoa but only accept positive numbers
uint32_t Oct2OctString(uint64_t num, char *octString);
uint32_t Oct2OctString(uint64_t num, char *octString, char sep);

// 把64位无符号整数转为其字符串表达式，比如 18 -> "0x12"
// 64位整数，最大为2^64 = 0xFFFF,FFFF,FFFF,FFFF，其字符串表示的最大长度为18，加上字符串尾的NULL字符，任何64位无符号整数都可以转
// 化为长度为19以内的字符串
// @param hexString, 存放结果，需要保证hexString的长度大于19
uint32_t Oct2HexString(uint64_t num, char *hexString);
uint32_t Oct2HexString(uint64_t num, char *hexString, char sep);

uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString);
uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString, char sep);

uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString);
uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString, char sep);
	
void RemoveLast(char *pc);

#endif

