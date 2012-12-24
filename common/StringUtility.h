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

// ���ַ��������ݵ��ã�����"abc" -> "cba"
void ReverseString(char *str);

// ��15���ڵ���תΪ16���Ƶı��ʽ������10 -> 'A',  15 -> 'F'
// @return �ɹ�����'0' - '9' 'A' - 'F', ʧ�ܷ���'W'��ʾwrong��ʧ�ܵ�ԭ������Ϊ���ִ���15
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
ÿ����1byte���ַ������ʽ��������3��0
n byte��ʮ�����ַ������ʽ С�ڵ��� n * 3��Ϊ����ַ���ĩβ��NULL + 1

1byte 2^8 = FF    					2
2byte 2^16 = FFFF    				4
3byte 2^24 = FFFF FF				5
4byte 2^32 = FFFF FFFF				8
5byte 2^40 = FFFF FFFF FF			10
6     2^48 = FFFF FFFF FFFF 		12
7     2^56 = FFFF FFFF FFFF FF		14
8     2^64 = FFFF FFFF FFFF FFFF	16
ÿ����1byte���ַ������ʽ����λ
n byte��ʮ�������ַ������ʽ С�ڵ��� n * 2��Ϊ����ַ���ĩβ��NULL + 1
*/

// ��64λ�޷�������תΪ���ַ������ʽ������ 987 -> "987"
// 64λ���ͣ����Ϊ2^64 = 2^4 * 2^30 * 2^30 = 16 * 1G * 1G ~ 16,000,000,000,000,000,000������uint64�ṹתΪ10���Ʊ��ʽ
// ����󳤶�Ϊ20λ���ټ�������NULL����ʾ�ַ�����β���κ�64λ�޷�������������ת��Ϊ����Ϊ21���ַ���
// @param octString, ��Ž������Ҫ��֤octString�ĳ��ȴ���21
// You can think this as itoa but only accept positive numbers
uint32_t Oct2OctString(uint64_t num, char *octString);
uint32_t Oct2OctString(uint64_t num, char *octString, char sep);

// ��64λ�޷�������תΪ���ַ������ʽ������ 18 -> "0x12"
// 64λ���������Ϊ2^64 = 0xFFFF,FFFF,FFFF,FFFF�����ַ�����ʾ����󳤶�Ϊ18�������ַ���β��NULL�ַ����κ�64λ�޷�������������ת
// ��Ϊ����Ϊ19���ڵ��ַ���
// @param hexString, ��Ž������Ҫ��֤hexString�ĳ��ȴ���19
uint32_t Oct2HexString(uint64_t num, char *hexString);
uint32_t Oct2HexString(uint64_t num, char *hexString, char sep);

uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString);
uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString, char sep);

uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString);
uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString, char sep);
	
void RemoveLast(char *pc);

#endif

