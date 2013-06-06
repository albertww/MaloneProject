
#include "gamephase.h"

CPreStartPhase::CPreStartPhase(CGameRound *gameround): m_Phase(GamePhase::PRESTART), m_GameRound(gameround)
{
}

CGamePhase *  CPreStartPhase::Perform()
{
  return m_GameRound->GetJudgePhase();
}

// ---------------------------------------------------


CJudgePhase::CJudgePhase(CGameRound *gameround): m_Phase(GamePhase::JUDGE), m_GameRound(gameround)
{
}

CGamePhase * CJudgePhase::Perform()
{
  return m_GameRound->GetDrawCardPhase();
}

// ---------------------------------------------------


CDrawCardPhase::CDrawCardPhase(CGameRound *gameround): m_Phase(GamePhase::DRAWCARD), m_GameRound(gameround)
{
}

CGamePhase * CDrawCardPhase::Perform()
{
  return m_GameRound->GetOutCardPhase();
}

// ---------------------------------------------------


COutCardPhase::COutCardPhase(CGameRound *gameround): m_Phase(GamePhase::OUTCARD), m_GameRound(gameround)
{
}

CGamePhase * COutCardPhase::Perform()
{
  return m_GameRound->GetThrowCardPhase();
}

// ---------------------------------------------------


CThrowCardPhase::CThrowCardPhase(CGameRound *gameround): m_Phase(GamePhase::THROWCARD), m_GameRound(gameround)
{
}

CGamePhase * CThrowCardPhase::Perform()
{
  return m_GameRound->SetPhase(m_GameRound->GetAfterThrowCardPhase());
}

// ---------------------------------------------------

CAfterThrowCardPhase::CAfterThrowCardPhase(CGameRound *gameround): m_Phase(GamePhase::AFTER_THROWCARD), m_GameRound(gameround)
{
}

CGamePhase * CAfterThrowCardPhase::Perform()
{
  return NULL;
}


// ---------------------------------------------------

CGameRound::CGameRound(): m_CurrentPhase(NULL)
{
  memset(m_Phase, 0, sizeof(m_Phase));
}

int CGameRound::Init()
{
  m_Phase[PRESTART] = new CPreStartPhase(this);
  m_Phase[JUDGE] = new CJudgePhase(this);
  m_Phase[DRAWCARD] = new CDrawCardPhase(this);
  m_Phase[OUTCARD] = new COutCardPhase(this);
  m_Phase[THROWCARD] = new CThrowCardPhase(this);
  m_Phase[AFTER_THROWCARD] = new CAfterThrowCardPhase(this);
  m_CurrentPhase = m_Phase[PRESTART];

  return 1;
}

void CGameRound::Run()
{
}

void CGameRound::SetPhase(CGamePhase *phase)
{
  m_CurrentPhase = phase;
}


