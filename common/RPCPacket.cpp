#include "RPCPacket.h"
#include <string.h>
#include <netinet/in.h> // htonl ntohl ... etc

//const int MAX_RPC_PACKET_LEN = 1024;
const char flags[4] = { 0xDD, 0xEE, 0x55, 0x99 };

// ------------------------------ CRPCPacket --------------------------------

CRPCPacket::CRPCPacket(): m_MaxBufferLen(MAX_RPC_PACKET_LEN), m_PacketLen(0)
{
	memset(m_PacketBuffer, 0, m_MaxBufferLen);
}

CRPCPacket::CRPCPacket(const char *data, int len): m_MaxBufferLen(MAX_RPC_PACKET_LEN), m_PacketLen(0)
{
	if (len <= m_MaxBufferLen)
	{
		memcpy(m_PacketBuffer, data, len);
		m_PacketLen = len;
	}
}

CRPCPacket::~CRPCPacket()
{

}

int CRPCPacket::GetPacketLen()
{
	return m_PacketLen;
}

void CRPCPacket::SetPacketLen(int len)
{
	m_PacketLen = len;
}

char * CRPCPacket::GetBuffer()
{
	return m_PacketBuffer;
}


// ------------------------------ CDataPacker --------------------------------

CDataPacker::CDataPacker(): m_IsValid(0), m_BufferLen(0), m_DataPtr(NULL), 
							m_CurrentPos(0), m_StartOffset(0), m_EndOffset(0),
							m_PacketLen(0)
{
}

CDataPacker::~CDataPacker()
{
}

int CDataPacker::SetBuffer(char *buffer, int len)
{
	m_IsValid = 0;
	if (!buffer || len <= 0)
		return 0;
	m_StartOffset = sizeof(int) + 2;
	m_CurrentPos = m_StartOffset;
	m_EndOffset = len - 2;
	// empty data packet is not allowed
	if (m_StartOffset >= m_EndOffset)
		return 0;
	m_DataPtr = buffer;
	m_IsValid = 1;
	return 1;
#if 0
	m_IsValid = 0;
	if (!buffer || len <= 0)
		return 0;
	m_StartOffset = sizeof(int) + 2;
	m_CurrentPos = m_StartOffset;
	m_EndOffset = len - 2;
	// empty data packet is not allowed
	if (m_StartOffset >= m_EndOffset)
		return 0;
	char *pflag = buffer + sizeof(int);
	if (*pflag++ != 0xDD || *pflag != 0xEE)
		return -1;
	int *pLen = (int *)buffer;
	m_PacketLen = ntohl(*pLen);
	if (m_PacketLen > len)
		return 0;
	pflag = buffer + m_PacketLen - 2;
	if (*pflag++ != 0x55 || *pflag != 0x99)
		return -1;
	m_EndOffset = m_PacketLen - 2;
	// not possible here
	if (m_StartOffset >= m_EndOffset)
		return -1;
	m_DataPtr = buffer;
	m_BufferLen = len;
	m_IsValid = 1;
	return 1;
#endif
}

int CDataPacker::WriteInt(int val)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(int) > m_EndOffset)
		return 0;
	int *pi = (int *)(m_DataPtr + m_CurrentPos);
	*pi = htonl(val);
	m_CurrentPos += sizeof(int);
	return 1;
}

int CDataPacker::WriteShort(short val)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(short) > m_EndOffset)
		return 0;
	short *ps = (short *)(m_DataPtr + m_CurrentPos);
	*ps = htons(val);
	m_CurrentPos += sizeof(short);
	return 1;
}

int CDataPacker::WriteByte(char c)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(char) > m_EndOffset)
		return 0;
	char *pc = (char *)(m_DataPtr + m_CurrentPos);
	*pc = c;
	m_CurrentPos += sizeof(char);
	return 1;
}

int CDataPacker::WriteBinary(const char *data, int count)
{
	if (!m_IsValid)
		return 0;
	if (!data || count <= 0)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + count > m_EndOffset)
		return 0;
	memcpy(m_DataPtr + m_CurrentPos, data, count);
	m_CurrentPos += count;
	return 1;
}

int CDataPacker::Finish()
{
	if (!m_IsValid)
		return 0;
	char *pflag = m_DataPtr + sizeof(int);
	*pflag++ = 0xDD;
	*pflag = 0xEE;
	pflag = m_DataPtr + m_CurrentPos;
	*pflag++ = 0x55;
	*pflag = 0x99;
	m_CurrentPos += 2;
	int *pLen = (int *)m_DataPtr;
	*pLen = htonl(m_CurrentPos);
	m_PacketLen = m_CurrentPos;
	return m_CurrentPos;
}

int CDataPacker::GetPacketLen()
{
	if (!m_IsValid)
		return 0;
	return m_PacketLen;
}

int CDataPacker::IsValid()
{
	return m_IsValid;
}


// ------------------------------ CDataUnPacker --------------------------------

CDataUnPacker::CDataUnPacker(): m_IsValid(0), m_BufferLen(0), m_DataPtr(NULL), 
							m_CurrentPos(0), m_StartOffset(0), m_EndOffset(0), m_PacketLen(0)
{
}

CDataUnPacker::~CDataUnPacker()
{
}

int CDataUnPacker::SetBuffer(char *buffer, int len)
{
	m_IsValid = 0;
	if (!buffer || len <= 0)
		return 0;
	m_StartOffset = sizeof(int) + 2;
	m_CurrentPos = m_StartOffset;
	m_EndOffset = len - 2;
	// empty data packet is not allowed
	if (m_StartOffset >= m_EndOffset)
		return 0;
	char *pflag = buffer + sizeof(int);
	if ((unsigned char)*pflag++ != 0xDD || (unsigned char)*pflag != 0xEE)
		return -1;
	int *pLen = (int *)buffer;
	m_PacketLen = ntohl(*pLen);
	if (m_PacketLen > len)
		return 0;
	pflag = buffer + m_PacketLen - 2;
	if ((unsigned char)*pflag++ != 0x55 || (unsigned char)*pflag != 0x99)
		return -1;
	m_EndOffset = m_PacketLen - 2;
	// not possible here
	if (m_StartOffset >= m_EndOffset)
		return -1;
	m_DataPtr = buffer;
	m_BufferLen = len;
	m_IsValid = 1;
	return 1;
}

int CDataUnPacker::ReadInt(int &val)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(int) > m_EndOffset)
		return 0;
	int *pi = (int *)m_DataPtr + m_CurrentPos;
	val = ntohl(*pi);
	m_CurrentPos += sizeof(int);
	return 1;
}

int CDataUnPacker::ReadShort(short &val)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(short) > m_EndOffset)
		return 0;
	short *ps = (short *)m_DataPtr + m_CurrentPos;
	val = ntohs(*ps);
	m_CurrentPos += sizeof(short);
	return 1;
}

int CDataUnPacker::ReadByte(char &c)
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + sizeof(char) > m_EndOffset)
		return 0;
	char *pc = (char *)m_DataPtr + m_CurrentPos;
	c = *pc;
	m_CurrentPos += sizeof(char);
	return 1;
}

int CDataUnPacker::ReadBinary(char *data, int count)
{
	if (!m_IsValid)
		return 0;
	if (!data || count <= 0)
		return 0;
	if (m_CurrentPos >= m_EndOffset || m_CurrentPos + count > m_EndOffset)
		return 0;
	memcpy(data, m_DataPtr + m_CurrentPos, count);
	m_CurrentPos += count;
	return 1;
}

int CDataUnPacker::HaveData()
{
	if (!m_IsValid)
		return 0;
	if (m_CurrentPos < m_EndOffset)
		return 1;
	else
		return 0;
}

int CDataUnPacker::GetPacketLen()
{
	if (!m_IsValid)
		return 0;
	return m_PacketLen;
}

int CDataUnPacker::IsValid()
{
	return m_IsValid;
}

CRPCPacket * CDataUnPacker::GenPacket()
{
	return new CRPCPacket(m_DataPtr, m_PacketLen);
}
