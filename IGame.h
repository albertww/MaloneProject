/*
  @Author: albert
  @Date: 2013-03-28
  @Description: This is interface for game
*/

#ifndef IGAME_H_
#define IGAME_H_

#include <iostream>

//属性
struct tagProperty
{
  int xxx;
};

//技能
class ISkill {
public:
  //技能动画时间
  virtual int GetSkillTime() = 0;
  //使用技能
  virtual int EmitSkill(ICharacter* pCharacter) = 0;
};

//装备
class IEquipment
{
public:
  //获取装备的类型
  virtual int GetEquipmentType() = 0;
  //获取装备的技能
  virtual ISkill* GetSkill() = 0;
  //获取装备的属性
  virtual tagProperty GetProperty() = 0;
};

class IHelmet: IEquipment
{
public:
  
};

class IArmor
{
public:

};

class IGlove
{
public:

};

class IShoe
{
public:

};


class IHead
{
public:
  virtual int GetHealthPoint() = 0;
  virtual int GetMaxHealthPoint() = 0;
  virtual int GetIntelligence() = 0;
  
protected:
  int health_point_;
  int max_health_point_;
  int intelligence_;
  IArmor *armor_of_head_;
};

class IHand
{
public:
  virtual int GetHealthPoint() = 0;
  virtual int GetMaxHealthPoint() = 0;
  virtual int GetPower() = 0;
  
protected:
  int health_point_;
  int max_health_point_;
  int power_;
  IGlove *glove_of_hand_;
};

class IBody
{
public:
  virtual int GetHealthPoint() = 0;
  virtual int GetMaxHealthPoint() = 0;
  
protected:
  int health_point_;
  int max_health_point_;
  IArmor *armor_of_body_;
}

class IFoot
{
public:
  virtual int GetHealthPoint() = 0;
  virtual int GetMaxHealthPoint() = 0;
  virtual int GetAgility() = 0;
  
protected:
  int health_point_;
  int max_health_point_;
  int agility_;
  IShoe *shoe_of_foot_;
};

enum HeroType
{
  HERO_UNKNOWN = 0,
  HERO_INTELLIGENCE,
  HERO_POWER,
  HERO_AGILITY
};

//角色
class ICharacter
{
public:
  //角色初始化
  virtual int InitCharacter() = 0;
  //穿装备
  virtual int Equip(IEquipment* pEquipment) = 0;
  //卸装备
  virtual int UnEquip(int iEquipmentType) = 0;
  //攻击
  virtual int Attack() = 0;
  //防御（被攻击）
  virtual int Defend() = 0;
  //virtual int UseBuff() = 0;
  //virtual int UseAttackSkill() = 0;
  //virtual int UseDefendSkill() = 0;
  // 计算伤害
  virtual int CalculateDamage();
  // 计算防御
  virtual int CalculateDefense();
  
protected:
  IHead *head_;
  IHand *left_hand_;
  IHand *right_hand_;
  IFoot *foot_;
  
  short hero_type_;
};

//场景
class IGameScene
{
public:
  //初始化场景
  virtual int InitGameScene() = 0;
  //向场景中加入角色
  virtual int JoinCharacter(ICharacter* pCharacter, int iTeam) = 0;
  //开始游戏
  virtual int StartGame() = 0;
  //结束游戏
  virtual int EndGame() = 0;
};


#endif /* defined(__hello_word__IGame__) */
