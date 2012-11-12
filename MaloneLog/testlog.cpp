
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
	MALONE_LOG(logger, DEBUG, "%s %i", "haha", 6);
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
	MALONE_DEBUG_LOGS(logger, "hahaha" << "I have a gold elephant" << 9999)

	return 0;
}
