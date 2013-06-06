/*
  Author: Albert
  Date: 20130417
  Description: Definition of card
*/

#ifndef _CARD_H_
#define _CARD_H_

#include "datatypes.h"

//
enum CardSuits
{
  SPADE = 1, //ºÚÌÒ
  HEART,     //ºìÌÒ
  DIAMOND,   //Ã·»¨
  CLUB       //·½¿é
};

enum CardPoint
{
  ONE = 1,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NIE,
  TEN,
  JACK,
  QUENE,
  KING
};

enum CardEffect
{
  
};

class CCard
{
public:
  CCard();

  uchar GetCardSuit() const { return m_CardSuit; }
  void SetCardSuit(uchar cardSuit) { m_CardSuit = cardSuit; }
  uchar GetCardPoint() const { return m_CardPoint; }
  void SetCardPoint(uchar cardPoint) { m_CardPoint = cardPoint; }
  

protected:
  uchar m_CardSuit;
  uchar m_CardPoint;
};

#endif
