
#include "MaloneLog.h"

CMaloneLog logger("Main");

class base
{
public:
	base() { MALONE_ERROR_LOG(logger, "this is base constructer"); }
};

int main()
{
	base b;
	/*
	MALONE_LOG(logger, DEBUG, "%s %i", "afdasfasdfdsafdsdsafdsafdsa", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_LOG(logger, DEBUG, "%s %i", "yaya", 6);
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 1)
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 2)
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 3)
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 4)
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 5)
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 6)*/
	LOG_DEBUG("%s %i", "afdasfasdfdsafdsdsafdsafdsa", 1);
	LOG_DEBUG("%s %i", "afdasfasdfdsafdsdsafdsafdsa", 2);
	LOG_DEBUG("%s %i", "afdasfasdfdsafdsdsafdsafdsa", 3);
	LOG_DEBUG("%s %i", "afdasfasdfdsafdsdsafdsafdsa", 4);
	LOG_DEBUG("%s %i", "afdasfasdfdsafdsdsafdsafdsa", 5);
	LOG_DEBUG_S("yohehe" << " sdfasdgaggggggggggeee " << 1000);
	LOG_DEBUG_S("yohehe" << " sdfasdgaggggggggggeee " << 2000);
	LOG_DEBUG_S("yohehe" << " sdfasdgaggggggggggeee " << 3000);
	LOG_DEBUG_S("yohehe" << " sdfasdgaggggggggggeee " << 4000);
	LOG_DEBUG_S("yohehe" << " sdfasdgaggggggggggeee " << 5000);

	return 0;
}
