#include <string>
#include <fstream>
#include "Star.h"
#include <cstring>
class CSVAdapter
{
public:
	CSVAdapter(std::string);
	~CSVAdapter();
    StarPoint getNextRecord();
    int appendNewRecord(StarPoint);
    StarPoint getSpecRecord(int);
	void setCursorBegin();
	bool hasRecord();
private:
	std::fstream CSVFile;
};

