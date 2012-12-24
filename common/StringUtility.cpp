#include "StringUtility.h"

char * Trim(char *ptr)
{
	int start, end, i;
	if (ptr && strlen(ptr) > 0)
	{
		for (start = 0; isspace(ptr[start]); start++)
			;
		for (end = strlen(ptr) - 1; end > 0 && isspace(ptr[end]); end--)
			;
		for (i = start; i <= end; i++)
			ptr[i - start] = ptr[i];
		ptr[end - start + 1] = '\0';
		return (ptr);
	}
	else
		return NULL;
}

int GetField(char *src, char *field, char *value)
{
	char buff[1024];
	value[0]=0;
	char *pc = NULL;
	pc = strstr(src, field);
	if (!pc)
		return 0;
	char *pc2 = pc + strlen(field);
	// this is for situation GetField("a 1 b 2 c", "c", value), value of field c is not exist
	if (*pc2 == 0)
		return 0;
	while (*pc2)
	{
		if (isspace(*pc2))
			pc2++;
		else
			break;
	}
	// this is for situation GetField("a 1 b 2 c   ", "c", value), value of field c is not exist
	if (*pc2 == 0)
		return 0;
	char *pc3 = pc2;
	while (*pc3)
	{
		if (isspace(*pc3))
		{
			break;
		}
		else
			pc3++;
	}
	int len = pc3 - pc2;
	strncpy(value, pc2, len);
	value[len] = 0;
	return 1;
}

int StrSplit(char *src, const char *sep, vector<string> &vec)
{
	char *pcNext = NULL;
	src = Trim(src);
	char *pcStart = src;
	char filed[512];
	while (*pcStart)
	{
		memset(filed, 0, sizeof(filed));
		pcNext = strstr(pcStart, sep);
		// no more pattern
		if (pcNext == NULL)
		{

			vec.push_back(pcStart);
			break;
		}
		// first is sep
		if (pcNext == pcStart)
		{
			pcStart = pcNext + 1;
			vec.push_back("");
		}
		else
		{
			int len = pcNext - pcStart;
			strncpy(filed, pcStart, len);
			filed[len + 1] = 0;
			pcStart =pcNext + 1;
			vec.push_back(filed);
		}
		
	}
	
	return vec.size();
}

int StrSplitBySpaces(char *src, vector<string> &vec)
{
	char *pcNext = NULL;
	src = Trim(src);
	char *pcStart = src;
	pcNext = src;
	char filed[512];
	while (*pcStart)
	{
		if (isspace(*pcStart))
		{
			pcStart++;
		}
		else
		{
			pcNext = pcStart;
			while (*pcNext)
			{
				if (isspace(*pcNext))
				{
					break;
				}
				else
				{
					pcNext++;
				}
			}
			memset(filed, 0, sizeof(filed));
			int len = pcNext - pcStart;
			strncpy(filed, pcStart, len);
			filed[len] = 0;
			pcStart = pcNext;
			vec.push_back(filed);
		}
	}
	return vec.size();
}

int StrJoin(char *dst, char sep, vector<string> &vec)
{
	if (vec.size() > 0)
	{
		int index = 0;
		char* pc = dst;
		for (index = 0; index < vec.size(); index++)
		{
			strcpy(pc, vec[index].c_str());
			pc += strlen(vec[index].c_str());
			if (index != vec.size() - 1)
			{
				*pc = sep;
			}
			pc++;
		}
		return pc - dst;
	}
	else
	{
		return 0;
	}
}


string WStringToString(const wstring& wstr)
{
	string result;
	const wchar_t* pwc = wstr.c_str();
	int wlen = wcslen(pwc);
	int buffLen = wlen * 2 + 1;
	char* pc = new char[buffLen];
	memset(pc, 0, buffLen);
#ifdef _WIN32
	WideCharToMultiByte(CP_ACP, 0, pwc, wlen, pc, buffLen, NULL, NULL);
#else
	//wcstombs(
	string oldLocale = setlocale(LC_CTYPE, "zh_CN.gbk");
	wcstombs(pc, pwc, buffLen);
	setlocale(LC_ALL, oldLocale.c_str());
#endif
	result = pc;
	delete [] pc;
	return result;
}

wstring StringToWString(const string& str)
{
	wstring result;
	const char* pc = str.c_str();
	int len = strlen(pc);
	int buffLen = len + 1;
	wchar_t* pwc = new wchar_t[buffLen];
	wmemset(pwc, 0, buffLen);
#ifdef _WIN32
	MultiByteToWideChar(CP_ACP, 0, pc, len, pwc, buffLen);
#else
	//mbstowcs(pwc, pc, len * 2);
	string oldLocale = setlocale(LC_CTYPE, "zh_CN.gbk");
	mbstowcs(pwc, pc, buffLen);
	setlocale(LC_ALL, oldLocale.c_str());
#endif
	result = pwc;
	delete [] pwc;
	return result;
}

string WStringToUTF8(const wstring& wstr)
{
	string result;
	const wchar_t* pwc = wstr.c_str();
	int wlen = wcslen(pwc);
	// length of chinese charactor in utf-8 is 3 bytes in average
	// some times 4 bytes is needed
	int buffLen = wlen * 4 + 1;
	char* pc = new char[buffLen];
	memset(pc, 0, buffLen);
#ifdef _WIN32
	WideCharToMultiByte(CP_UTF8, 0, pwc, wlen, pc, buffLen, NULL, NULL);
#else
	//wcstombs(
	string oldLocale = setlocale(LC_CTYPE, "zh_CN.utf8");
	wcstombs(pc, pwc, buffLen);
	setlocale(LC_ALL, oldLocale.c_str());
#endif
	result = pc;
	delete [] pc;
	return result;
}

wstring UTF8ToWString(const string& str)
{
	wstring result;
	const char* pc = str.c_str();
	int len = strlen(pc);
	int buffLen = len + 1;
	wchar_t* pwc = new wchar_t[buffLen];
	wmemset(pwc, 0, buffLen);
#ifdef _WIN32
	MultiByteToWideChar(CP_ACP, 0, pc, len, pwc, buffLen);
#else
	//mbstowcs(pwc, pc, len * 2);
	string oldLocale = setlocale(LC_CTYPE, "zh_CN.utf8");
	mbstowcs(pwc, pc, buffLen);
	setlocale(LC_ALL, oldLocale.c_str());
#endif
	result = pwc;
	delete [] pwc;
	return result;
}

string WChar2Ansi(wstring szWideStr)
{
    std::string curLocale = setlocale(LC_CTYPE,"zh_CH.gbk");
	//    setlocale(LC_ALL, "chs");
    const wchar_t* pSrc = szWideStr.c_str();
    size_t szDest = 2 * szWideStr.size() + 1;
    char *pDest = new char[szDest];
    memset(pDest,0,szDest);
    int tmpSize = wcstombs(pDest,pSrc,szDest);
	//	cout<<"size1:"<<tmpSize<<",size2:"<<szDest<<",data:"<<pDest<<endl;
    std::string result = pDest;
    delete []pDest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

wstring Ansi2WChar(string szAscStr)
{
    std::string curLocale = setlocale(LC_CTYPE,"zh_CH.gbk");
	std::wstring result=L"";
	const char* pSrc = szAscStr.c_str();
	size_t szDest = szAscStr.size() + 1;
    wchar_t *pDest = new wchar_t[szDest];
    wmemset(pDest, 0, szDest);
    int tmpSize = mbstowcs(pDest,pSrc,szDest);
    result = pDest;
    delete []pDest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

void ReverseString(char *str)
{
	if (str == NULL || strlen(str) <= 1)
		return;
	int len = strlen(str);
	int lastind = len - 1;
	int index = 0;
	for (index = 0; index < len / 2; index++)
	{
		char tmp = str[index];
		str[index] = str[lastind - index];
		str[lastind - index] = tmp;
	}
	return;
}

char Digit2Hex(uint64_t val)
{
	if (val >= 0 && val < 10)
	{
		return (char)('0' + val);
	}
	else if (val < 16)
	{
		return (char)('A' + (val - 10));
	}
	else
		return 'w';
}

uint32_t Oct2OctString(uint64_t num, char *octString)
{
	if (octString == NULL)
		return 0;
	if (num == 0)
	{
		strcpy(octString, "0");
		return 1;
	}
	uint64_t temp = num;
	uint32_t count = 0;
	int ind = 0;
	while (temp > 0)
	{
		octString[ind] = (char)(temp % 10) + '0';
		temp = temp / 10;
		ind++;
	}
	octString[ind] = 0;
	ReverseString(octString);
	count = strlen(octString);
	return count;
}

uint32_t Oct2OctString(uint64_t num, char *octString, char sep)
{
	if (octString == NULL)
		return 0;
	uint32_t count = Oct2OctString(num, octString);
	char *plast = octString + count;
	*plast++ = sep;
	*plast = 0;
	count++;
	return count;
}

uint32_t Oct2HexString(uint64_t num, char *hexString)
{
	if (hexString == NULL)
		return 0;
	if (num == 0)
	{
		strcpy(hexString, "0x00");
		return 4;
	}
	uint64_t temp = num;
	uint32_t count = 0;
	int ind = 0;
	while (temp > 0)
	{
		uint8_t val = temp % 16;
		hexString[ind] = Digit2Hex(val);
		temp = temp / 16;
		ind++;
	}
	if (ind % 2 == 1)
	{
		hexString[ind++] = '0';
	}
	hexString[ind++] = 'x';
	hexString[ind++] = '0';
	hexString[ind++] = 0;
	ReverseString(hexString);
	count = strlen(hexString);
	return count;
}

uint32_t Oct2HexString(uint64_t num, char *hexString, char sep)
{
	if (hexString == NULL)
		return 0;
	uint32_t count = Oct2HexString(num, hexString);
	char *plast = hexString + count;
	*plast++ = sep;
	*plast = 0;
	count++;
	return count;
}

uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString)
{
	if (byteString == NULL || datas == NULL)
		return 0;
	uint32_t count = 0;
	uint32_t current = 0;
	int index = 0;
	uint8_t *pc = datas;
	char *pd = byteString;
	for (index = 0; index < len; index++)
	{
		current = Oct2OctString(*pc, pd);
		pd += current;
		count += current;
		pc++;
	}
	return count;
}

uint32_t Byte2OctString(uint8_t *datas, uint32_t len, char *byteString, char sep)
{
	if (byteString == NULL || datas == NULL)
		return 0;
	uint32_t count = 0;
	uint32_t current = 0;
	int index = 0;
	uint8_t *pc = datas;
	char *pd = byteString;
	for (index = 0; index < len; index++)
	{
		current = Oct2OctString(*pc, pd, sep);
		pd += current;
		count += current;
		pc++;
	}
	return count;
}

uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString)
{
	if (byteString == NULL || datas == NULL)
		return 0;
	uint32_t count = 0;
	uint32_t current = 0;
	int index = 0;
	uint8_t *pc = datas;
	char *pd = byteString;
	for (index = 0; index < len; index++)
	{
		current = Oct2HexString(*pc, pd);
		pd += current;
		count += current;
		pc++;
	}
	return count;
}

uint32_t Byte2HexString(uint8_t *datas, uint32_t len, char *byteString, char sep)
{
	if (byteString == NULL || datas == NULL)
		return 0;
	uint32_t count = 0;
	uint32_t current = 0;
	int index = 0;
	uint8_t *pc = datas;
	char *pd = byteString;
	for (index = 0; index < len; index++)
	{
		current = Oct2HexString(*pc, pd, sep);
		pd += current;
		count += current;
		pc++;
	}
	return count;
}

void RemoveLast(char *pc)
{
	if (pc == NULL || strlen(pc) == 0)
		return;
	char *last = pc + strlen(pc) - 1;
	*last = 0;
}
