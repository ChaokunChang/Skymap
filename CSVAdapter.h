#include <string>
#include <fstream>
#include "Star.h"

class CSVAdapter
{
public:
	CSVAdapter(std::string);
	~CSVAdapter();
	Star getNextRecord();
	int appendNewRecord(Star);
	Star getSpecRecord(int);
	void setCursorBegin();
	bool hasRecord();
private:
	std::fstream CSVFile;
};

