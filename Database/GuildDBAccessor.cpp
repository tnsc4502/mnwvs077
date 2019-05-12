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

		WvsLogger::LogFormat("Create GuildInfo : %s\n", queryStatement.toString().c_str());
		queryStatement.execute();
		queryStatement.reset(GET_DB_SESSION);

		queryStatement << "INSERT INTO GuildMember(GuildID, WorldID, CharacterID, CharacterName, Job, Level, Grade, Contribution) VALUES";

		for (int i = 0; i < pGuild->anCharacterID.size(); ++i)
		{
			queryStatement << "("
				<< pGuild->nGuildID << ", "
				<< nWorldID << ", "
				<< pGuild->anCharacterID[i] << ", "
				<< "'" << pGuild->aMemberData[i].sCharacterName << "', "
				<< pGuild->aMemberData[i].nJob << ", "
				<< pGuild->aMemberData[i].nLevel << ", "
				<< pGuild->aMemberData[i].nGrade << ", "
				<< pGuild->aMemberData[i].nContribution << ") ";
			if (i != pGuild->anCharacterID.size() - 1)
				queryStatement << ", ";
		}

		queryStatement << " ON DUPLICATE KEY UPDATE CharacterID = VALUES (CharacterID), "
			<< "`GuildID`=VALUES(`GuildID`),"
			<< "`Grade`=VALUES(`Grade`),"
			<< "`Contribution`=VALUES(`Contribution`)";

		WvsLogger::LogFormat("Create GuildMember : %s\n", queryStatement.toString().c_str());
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::MySQLException& se) 
	{
		WvsLogger::LogFormat("Create Guild Failed : %s\n", se.message());
	}
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

	queryStatement << " ON DUPLICATE KEY UPDATE CharacterID = VALUES (CharacterID), "
		<< "`GuildID`=VALUES(`GuildID`),"
		<< "`Grade`=VALUES(`Grade`),"
		<< "`Contribution`=VALUES(`Contribution`)";

	queryStatement.execute();
}

void * GuildDBAccessor::LoadGuild(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT GuildID From GuildMember Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return nullptr;

	int nGuildID = recordSet["GuildID"];
	GuildMan::GuildData* pGuild = AllocObj(GuildMan::GuildData);
	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT * From GuildInfo Where GuildID = " << nGuildID;
	queryStatement.execute();
	recordSet.reset(queryStatement);

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
