#include "Consensus.h"
#include <mutex>

// #include "/usr/local/include/boost/thread/thread.hpp"

using namespace std;

class Ecits {
public:
	Ecits();
	~Ecits();
	int count_replies();
private:
	long mIdCounter;
};

