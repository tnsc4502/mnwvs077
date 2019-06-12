#include "GuildBBSDBAccessor.h"
#include "WvsUnified.h"
#include "Poco\Data\MySQL\MySQLException.h"
#include "..\WvsCenter\GuildBBSMan.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

void GuildBBSDBAccessor::RegisterEntry(int nWorldID, int nGuildID, int nCharacterID, const std::string& sTitle, const std::string& sText, int nEmoticon, bool bNotice)
{
	int nNextEntryID = bNotice ? 0 : GetNextEntryID(nWorldID, nGuildID);
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO BBSEntry (GuildID, WorldID, EntryID, CharacterID, Emoticon, Title, Text, IsNotice, Date) VALUES(";
	queryStatement << nGuildID << ", "
		<< nWorldID << ", "
		<< nNextEntryID << ", "
		<< nCharacterID << ", "
		<< nEmoticon << ", "
		<< "'" << sTitle << "', "
		<< "'" << sText << "', "
		<< (bNotice ? 1 : 0) << ", "
		<< GameDateTime::GetCurrentDate() << ")";
	WvsLogger::LogFormat("Query = %s\n", queryStatement.toString().c_str());
	queryStatement.execute();
}

int GuildBBSDBAccessor::QueryEntryOwner(int nWorldID, int nGuildID, int nEntryID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	queryStatement << "SELECT CharacterID From BBSEntry Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet["CharacterID"].isEmpty())
		return -1;

	return (int)recordSet["CharacterID"];
}

void GuildBBSDBAccessor::DeleteEntry(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, bool bForce)
{
	if (!bForce && nCharacterID != QueryEntryOwner(nWorldID, nGuildID, nEntryID))
		return;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM BBSComment WHERE WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	queryStatement.execute();
	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "DELETE FROM BBSEntry WHERE WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	queryStatement.execute();
}

void GuildBBSDBAccessor::ModifyEntry(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, const std::string & sTitle, const std::string & sText, int nEmoticon, bool bForce)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE BBSEntry Set "
		<< "Title = '" << sTitle << "', "
		<< "Text = '" << sText << "', "
		<< "Emoticon = " << nEmoticon << ", "
		<< "Date = " << GameDateTime::GetCurrentDate() << " WHERE WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	if (!bForce)
		queryStatement << " AND CharacterID = " << nCharacterID;
	queryStatement.execute();
}

void GuildBBSDBAccessor::RegisterComment(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, const std::string& sComment)
{
	int nNextSN = GetNextCommentSN(nWorldID, nGuildID, nEntryID);
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO BBSComment (GuildID, EntryID, WorldID, SN, CharacterID, Comment, Date) VALUES(";
	queryStatement << nGuildID << ", "
		<< nEntryID << ", "
		<< nWorldID << ", "
		<< nNextSN << ", "
		<< nCharacterID << ", "
		<< "'" << sComment << "', "
		<< GameDateTime::GetCurrentDate() << ")";
	queryStatement.execute();
}

void GuildBBSDBAccessor::DeleteComment(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, int nCommentSN, bool bForce)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM BBSComment WHERE WorldID = " << nWorldID
		<< " AND GuildID = " << nGuildID
		<< " AND EntryID = " << nEntryID
		<< " AND SN = " << nCommentSN;
	if (!bForce)
		queryStatement << " AND CharacterID = " << nCharacterID;
	queryStatement.execute();
}

std::vector<void*> GuildBBSDBAccessor::LoadList(int nWorldID, int nGuildID)
{
	std::vector<void*> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	Poco::Data::Statement queryCount(GET_DB_SESSION);
	queryStatement << "SELECT * From BBSEntry Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " ORDER BY IsNotice DESC, EntryID DESC";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		queryCount.reset(GET_DB_SESSION);
		auto pEntry = AllocObj(GuildBBSMan::BBSEntry);
		pEntry->m_liDate = recordSet["Date"];
		pEntry->m_nEmoticon = recordSet["Emoticon"];
		pEntry->m_nCharacterID = recordSet["CharacterID"];
		pEntry->m_sTitle = recordSet["Title"].toString();
		pEntry->m_bIsNotice = ((int)recordSet["IsNotice"] == 1);
		pEntry->m_nEntryID = recordSet["EntryID"];
		aRet.push_back(pEntry);

		queryCount << "SELECT SN From BBSComment WHERE WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << pEntry->m_nEntryID;
		queryCount.execute();
		Poco::Data::RecordSet countRecordSet(queryCount);
		pEntry->m_nCommentCount = (int)countRecordSet.rowCount();
	}
	return aRet;
}

void* GuildBBSDBAccessor::LoadEntryView(int nWorldID, int nGuildID, int nEntryID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * From BBSEntry Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return nullptr;

	auto pEntry = AllocObj(GuildBBSMan::BBSEntry);
	pEntry->m_liDate = recordSet["Date"];
	pEntry->m_nEmoticon = recordSet["Emoticon"];
	pEntry->m_nCharacterID = recordSet["CharacterID"];
	pEntry->m_sTitle = recordSet["Title"].toString();
	pEntry->m_bIsNotice = ((int)recordSet["IsNotice"] == 1);
	pEntry->m_nEntryID = recordSet["EntryID"];
	pEntry->m_sText = recordSet["Text"].toString();

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT * From BBSComment WHERE WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID << " ORDER BY SN ASC";
	queryStatement.execute();
	recordSet.reset(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GuildBBSMan::BBSComment comment;
		comment.m_sComment = recordSet["Comment"].toString();
		comment.m_liDate = recordSet["Date"];
		comment.m_nSN = recordSet["SN"];
		comment.m_nCharacterID = recordSet["CharacterID"];
		pEntry->m_aComment.push_back(std::move(comment));
	}
	return pEntry;
}

int GuildBBSDBAccessor::GetNextEntryID(int nWorldID, int nGuildID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	queryStatement << "SELECT MAX(EntryID) From BBSEntry Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet["MAX(EntryID)"].isEmpty())
		return 1;

	int result = (int)recordSet["MAX(EntryID)"] + 1;
	return result;
}

int GuildBBSDBAccessor::GetNextCommentSN(int nWorldID, int nGuildID, int nEntryID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT MAX(SN) From BBSComment Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = " << nEntryID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0 || recordSet["MAX(SN)"].isEmpty())
		return 1;

	int result = (int)recordSet["MAX(SN)"] + 1;
	return result;
}

bool GuildBBSDBAccessor::IsNoticeExist(int nWorldID, int nGuildID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT EntryID From BBSEntry Where WorldID = " << nWorldID << " AND GuildID = " << nGuildID << " AND EntryID = 0";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	return recordSet.rowCount() != 0;
}