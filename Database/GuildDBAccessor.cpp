#include "GuildDBAccessor.h"
#include "Poco\Data\MySQL\MySQLException.h"

#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsGame\GuildMan.h"

GuildDBAccessor::GuildDBAccessor()
{
}

GuildDBAccessor::~GuildDBAccessor()
{
}

GuildDBAccessor * GuildDBAccessor::GetInstance()
{
	static GuildDBAccessor* sPtrAccessor = new GuildDBAccessor();
	return sPtrAccessor;
}

int GuildDBAccessor::GetGuildIDCounter()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	queryStatement << "SELECT MAX(GuildID) From GuildInfo";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet["MAX(GuildID)"].isEmpty())
		return 2;

	int result = (int )recordSet["MAX(GuildID)"];
	return result;
}

void GuildDBAccessor::CreateNewGuild(void *pGuild_, int nWorldID)
{
	try 
	{
		auto pGuild = (GuildMan::GuildData*)pGuild_;

		UpdateGuild(pGuild, nWorldID);
		for (int i = 0; i < pGuild->anCharacterID.size(); ++i)
		{
			JoinGuild(
				&(pGuild->aMemberData[i]),
				pGuild->anCharacterID[i],
				pGuild->nGuildID,
				nWorldID
			);
		}
	}
	catch (Poco::Data::MySQL::MySQLException& se) 
	{
		WvsLogger::LogFormat("Create Guild Failed : %s\n", se.message());
	}
}

void GuildDBAccessor::UpdateGuild(void * pGuild_, int nWorldID)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO GuildInfo (GuildID, WorldID, GuildName, MaxMemberNum, MarkBg, MarkBgColor, Mark, MarkColor, Notice, Point, GradeName1, GradeName2, GradeName3, GradeName4, GradeName5) VALUES(";
	queryStatement << pGuild->nGuildID << ", "
		<< nWorldID << ", "
		<< "'" << pGuild->sGuildName << "', "
		<< pGuild->nMaxMemberNum << ", "
		<< pGuild->nMarkBg << ","
		<< pGuild->nMarkBgColor << ","
		<< pGuild->nMark << ","
		<< pGuild->nMarkColor << ","
		<< "'" << pGuild->sNotice << "' ,"
		<< pGuild->nPoint << ","
		<< "'" << pGuild->asGradeName[0] << "' ,"
		<< "'" << pGuild->asGradeName[1] << "' ,"
		<< "'" << pGuild->asGradeName[2] << "' ,"
		<< "'" << pGuild->asGradeName[3] << "' ,"
		<< "'" << pGuild->asGradeName[4] << "')";

	queryStatement << " ON DUPLICATE KEY UPDATE "
		<< "GuildName = '" << pGuild->sGuildName << "', "
		<< "MaxMemberNum = " << pGuild->nMaxMemberNum << ", "
		<< "MarkBg = " << pGuild->nMarkBg << ", "
		<< "MarkBgColor = " << pGuild->nMarkBgColor << ", "
		<< "Mark = " << pGuild->nMark << ", "
		<< "MarkColor = " << pGuild->nMarkColor << ", "
		<< "Mark = " << pGuild->nMark << ", "
		<< "Notice = '" << pGuild->sNotice << "', "
		<< "Point = " << pGuild->nPoint << ", "
		<< "GradeName1 = '" << pGuild->asGradeName[0] << "', "
		<< "GradeName2 = '" << pGuild->asGradeName[1] << "', "
		<< "GradeName3 = '" << pGuild->asGradeName[2] << "', "
		<< "GradeName4 = '" << pGuild->asGradeName[3] << "', "
		<< "GradeName5 = '" << pGuild->asGradeName[4] << "'";
	queryStatement.execute();
}

void GuildDBAccessor::UpdateGuildMember(void *pMemberData_, int nCharacterID, int nGuildID, int nWorldID)
{
	auto pMemberData = (GuildMan::MemberData*)pMemberData_;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	queryStatement << "UPDATE GuildMember Set " <<
		"CharacterName = '" << pMemberData->sCharacterName << "', "
		"Grade = " << pMemberData->nGrade << ", " <<
		"Contribution = " << pMemberData->nContribution << ", " <<
		"Job = " << pMemberData->nJob << ", " <<
		"Level = " << pMemberData->nLevel 

		<< " WHERE CharacterID = " << nCharacterID 
		<< " AND GuildID = " << nGuildID 
		<< " AND WorldID = " << nWorldID;

	WvsLogger::LogFormat("Update GuildMember : %s\n", queryStatement.toString().c_str());
	queryStatement.execute();
}

std::vector<void*> GuildDBAccessor::LoadAllGuild(int nWorldID)
{
	std::vector<void*> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT GuildID From GuildID Where WorldID = " << nWorldID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement); (queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aRet.push_back(LoadGuild((int)recordSet["GuildID"]));
	
	return aRet;
}

void GuildDBAccessor::JoinGuild(void * pMemberData_, int nCharacterID, int nGuildID, int nWorldID)
{
	auto pMemberData = (GuildMan::MemberData*)pMemberData_;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO GuildMember(GuildID, WorldID, CharacterID, CharacterName, Job, Level, Grade, Contribution) VALUES";

	queryStatement << "("
		<< nGuildID << ", "
		<< nWorldID << ", "
		<< nCharacterID << ", "
		<< "'" << pMemberData->sCharacterName << "', "
		<< pMemberData->nJob << ", "
		<< pMemberData->nLevel << ", "
		<< pMemberData->nGrade << ", "
		<< pMemberData->nContribution << ") ";

	//WvsLogger::LogFormat("Join GuildMember : %s\n", queryStatement.toString().c_str());
	queryStatement.execute();
}

void GuildDBAccessor::WithdrawGuild(int nCharacterID, int nGuildID, int nWorldID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE From GuildMember Where CharacterID = " 
		<< nCharacterID << " AND GuildID = " << nGuildID << " AND WorldID = " << nWorldID;
	queryStatement.execute();
}

void GuildDBAccessor::RemoveGuild(int nGuildID, int nWorldID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE From GuildInfo Where GuildID = " << nGuildID << " AND WorldID = " << nWorldID;
	queryStatement.execute();
}

int GuildDBAccessor::LoadGuildID(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT GuildID From GuildMember Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return -1;

	return (int)recordSet["GuildID"];
}

void * GuildDBAccessor::LoadGuildByCharID(int nCharacterID)
{
	int nGuildID = LoadGuildID(nCharacterID);
	if (nGuildID == -1)
		return nullptr;
	
	return LoadGuild(nGuildID);
}

void * GuildDBAccessor::LoadGuild(int nGuildID)
{
	GuildMan::GuildData* pGuild = AllocObj(GuildMan::GuildData);
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * From GuildInfo Where GuildID = " << nGuildID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement); (queryStatement);

	pGuild->nGuildID = recordSet["GuildID"];
	pGuild->sGuildName = recordSet["GuildName"].toString();
	pGuild->nMaxMemberNum = recordSet["MaxMemberNum"];
	pGuild->nMarkBg = recordSet["MarkBg"];
	pGuild->nMarkBgColor = recordSet["MarkBgColor"];
	pGuild->nMark = recordSet["Mark"];
	pGuild->nMarkColor = recordSet["MarkColor"];
	pGuild->sNotice = recordSet["Notice"].toString();
	pGuild->nPoint = recordSet["Point"];

	pGuild->asGradeName.resize(5);
	pGuild->asGradeName[0] = recordSet["GradeName1"].toString();
	pGuild->asGradeName[1] = recordSet["GradeName2"].toString();
	pGuild->asGradeName[2] = recordSet["GradeName3"].toString();
	pGuild->asGradeName[3] = recordSet["GradeName4"].toString();
	pGuild->asGradeName[4] = recordSet["GradeName5"].toString();

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT * From GuildMember Where GuildID = " << nGuildID; 
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GuildMan::MemberData memberData;
		memberData.sCharacterName = recordSet["CharacterName"].toString();
		memberData.bOnline = false;
		memberData.nGrade = recordSet["Grade"];
		memberData.nJob = recordSet["Job"];
		memberData.nLevel = recordSet["Level"];
		memberData.nContribution = recordSet["Contribution"];

		pGuild->anCharacterID.push_back(recordSet["CharacterID"]);
		pGuild->aMemberData.push_back(std::move(memberData));
	}
	return pGuild;
}
