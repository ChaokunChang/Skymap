#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <ios>
#include <vector>
#include "CSVAdapter.h"

using namespace std;
CSVAdapter::CSVAdapter(std::string FileName)
{
	CSVFile.open(FileName);
}

bool CSVAdapter::hasRecord() {
    if(CSVFile.eof()) return false;
    else return true;
}


CSVAdapter::~CSVAdapter()
{
	CSVFile.close();
}

Star CSVAdapter::getNextRecord()
{
	std::string line;
	std::vector<std::string> fields;
	Star starRec0(0,0,0,0);
	if (!hasRecord())
	{
		//throw std::exception("CSVAdapter has reached the end of the file.");
		cout<<"end of file"<<endl;
	}
	else
	{
		std::getline(CSVFile, line);
		std::istringstream sin(line);
		std::string fieldBuf;

		if(line == "") return starRec0;
		while (std::getline(sin, fieldBuf, ','))
		{
			fields.push_back(fieldBuf);
		}
		Star starRec(atoi(fields[0].c_str()), atof(fields[1].c_str()), atof(fields[2].c_str()), atof(fields[3].c_str()));
		return starRec;
	}
	return starRec0;
}

int CSVAdapter::appendNewRecord(Star starRec)
{
	std::streampos cur = CSVFile.tellg();
	CSVFile.seekp(0, std::ios::end);
	CSVFile << starRec.getID() << ',' << starRec.getX() << ',' << starRec.getY() << ',' << starRec.getMag() << std::endl;
	CSVFile.seekp(cur);
	return 0;
}

Star CSVAdapter::getSpecRecord(int key)
{
	std::streampos cur = CSVFile.tellg();
	CSVFile.seekg(0, std::ios::beg);
	std::string line;
	std::vector<std::string> fields;
	while (!CSVFile.eof())
	{
		std::getline(CSVFile, line);
		std::istringstream sin(line);
		std::string fieldBuf;
		while (std::getline(sin, fieldBuf, ','))
		{
			fields.push_back(fieldBuf);
		}
		Star starRec(atoi(fields[0].c_str()), atof(fields[1].c_str()), atof(fields[2].c_str()), atof(fields[3].c_str()));
		if (starRec.getID() == key)
		{
			CSVFile.seekg(cur);
			return starRec;
		}
		else
		{
			fields.clear();
		}
	}
	CSVFile.seekg(cur);
	//throw std::exception("No Record match the provided key.");
	cout<<"No record"<<endl;
}

void CSVAdapter::setCursorBegin()
{
	CSVFile.seekg(0, std::ios::beg);
	CSVFile.seekp(0, std::ios::beg);
}