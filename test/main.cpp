#include "common.h"
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include "Stats.h"
#include "ThreadPool.h"

bool done = false;

int main(int argc, char **argv) {
	string threadStr(argv[1]), file_name(argv[2]);
	Utility utility;
	int thread_count = utility.getThreadCount(threadStr);
	Stats stats;
	ThreadPool tPool = ThreadPool();
	tPool.letTheGameBegin(std::ref(stats), thread_count, file_name);
}