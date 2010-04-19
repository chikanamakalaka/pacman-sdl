/*
 * PacmanDB.hpp
 *
 *  Created on: Oct 20, 2009
 *      Author: asantos
 */

#ifndef PACMANDB_HPP_
#define PACMANDB_HPP_

#include "sqlite3/sqlite3.h"
#include <boost/bimap.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

class SoundBinding{
	std::string event;
	std::string filename;
public:
	SoundBinding(const std::string& event, const std::string& filename):event(event), filename(filename){}
	const std::string& GetEvent()const{
		return event;
	}
	const std::string& GetFilename()const{
		return filename;
	}
};
class MusicBinding{
	std::string event;
	std::string filename;
	int fadeintime;
	int fadeouttime;
public:
	MusicBinding(const std::string& event, const std::string& filename, int fadeintime, int fadeouttime):event(event), filename(filename), fadeintime(fadeintime), fadeouttime(fadeouttime){}
	const std::string& GetEvent()const{
		return event;
	}
	const std::string& GetFilename()const{
		return filename;
	}
	const int GetFadeInTime()const{
		return fadeintime;
	}
	const int GetFadeOutTime()const{
		return fadeouttime;
	}
};
class PacmanDBException : public virtual std::exception{
private:
		const char* msg;
public:
	PacmanDBException(const char* msg):msg(msg){}
	char const* what()const throw(){
		return msg;
	}
};

class PacmanDB{
private:
	sqlite3 *db;
	int rc;
public:

	class Score{
		int score;
		std::string name;
	public:
		Score(int score, const std::string& name):score(score), name(name){}
		int GetScore()const{
			return score;
		}
		const std::string GetName()const{
			return name;
		}
	};

	PacmanDB(const std::string& dbname = FileSystem::MakeVarGamesPath("/pacman.db")):rc(sqlite3_open(dbname.c_str(), &db)){
		if(rc){
			const char* zErrMsg = sqlite3_errmsg(db);
			sqlite3_close(db);
			throw PacmanDBException(zErrMsg);
		}
	}
	~PacmanDB(){
		sqlite3_close(db);
	}

	std::list<Score> HighScores(){
		//See: http://souptonuts.sourceforge.net/readme_sqlite_tutorial.html
		std::list<Score> results;

		char* zErrMsg = 0;
	    char** result ;
	    int rc = 0;
	    int nrow,ncol;
	    std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT * FROM HighScores");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int scoreidx, nameidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Score"){
				scoreidx = idx;
			}else if(*itr=="Name"){
				nameidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			int score = boost::lexical_cast<int>(*(itr+scoreidx));
			std::string name = *(itr+nameidx);

			results.push_back(Score(score, name));
		}

		return results;
	}


	void AddScore(int score, const std::string& name){
		sqlite3 *db;
		sqlite3_stmt *res;
		int rc;
		rc = sqlite3_open("pacman.db", &db);
		if(rc){
			const char* zErrMsg = sqlite3_errmsg(db);
			sqlite3_close(db);
			throw PacmanDBException(zErrMsg);
		}
		std::string sql = (boost::format("INSERT INTO Scores VALUES(NULL, %1%, '%2%');")%score%name).str();

		const char *s = NULL;
		rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &res, &s);
		if (rc != SQLITE_OK)
		{
			return;
		}
		if (!res)
		{
			return;
		}
		rc = sqlite3_step(res); // execute
		sqlite3_finalize(res); // deallocate statement
		res = NULL;
		switch (rc)
		{
		case SQLITE_BUSY:
			return;
		case SQLITE_DONE:
		case SQLITE_ROW:
			return;
		case SQLITE_ERROR:
			return;
		case SQLITE_MISUSE:
			return;
		}
		if(rc!=SQLITE_OK){
			throw PacmanDBException("Error in PacmanDB.");
		}
		sqlite3_close(db);
	}

	bool QualifiesForHighScore(int score){
		std::list<Score> scores(HighScores());
		for(std::list<Score>::const_iterator itr=scores.begin();itr!=scores.end();itr++){
			if(itr->GetScore()<score){
				return true;
			}
		}
		if(scores.size()<10){
			return true;
		}
		return false;
	}

	boost::bimap<int, std::string> GetKeyBindings(){
		boost::bimap<int, std::string> keybindings;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT Key, Name FROM KeyBindings");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int nameidx, keyidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Name"){
				nameidx = idx;
			}
			else if(*itr=="Key"){
				keyidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			std::string name = *(itr+nameidx);
			int key = boost::lexical_cast<int>(*(itr+keyidx));

			keybindings.insert(boost::bimap<int,std::string>::value_type(key, name));
		}

		return keybindings;
	}
	std::string GetKeyNameByValue(int value){
		std::string name;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql((boost::format("SELECT Name FROM Keys WHERE Key = %1%")%value).str());

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int nameidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Name"){
				nameidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			name = *(itr+nameidx);
		}

		return name;
	}


	int GetVolumeValueByName(const std::string& name){
		int volume = 0;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql((boost::format("SELECT Volume FROM Volumes WHERE Name = '%1%'")%name).str());

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int volumeidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Volume"){
				volumeidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			volume = boost::lexical_cast<int>(*(itr+volumeidx));
		}

		return volume;
	}
	void SetVolume(const std::string& name, int volume){
		sqlite3 *db;
		sqlite3_stmt *res;
		int rc;
		rc = sqlite3_open("pacman.db", &db);
		if(rc){
			const char* zErrMsg = sqlite3_errmsg(db);
			sqlite3_close(db);
			throw PacmanDBException(zErrMsg);
		}
		std::string sql = (boost::format("UPDATE Volumes SET Volume = %1% WHERE Name = '%2%'")%volume%name).str();

		const char *s = NULL;
		rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &res, &s);
		if (rc != SQLITE_OK)
		{
			return;
		}
		if (!res)
		{
			return;
		}
		rc = sqlite3_step(res); // execute
		sqlite3_finalize(res); // deallocate statement
		res = NULL;
		switch (rc)
		{
		case SQLITE_BUSY:
			return;
		case SQLITE_DONE:
		case SQLITE_ROW:
			return;
		case SQLITE_ERROR:
			return;
		case SQLITE_MISUSE:
			return;
		}
		if(rc!=SQLITE_OK){
			throw PacmanDBException("Error in PacmanDB.");
		}
		sqlite3_close(db);
	}

	void SetKeyBinding(const std::string& name, int keyvalue){
		sqlite3 *db;
		sqlite3_stmt *res;
		int rc;
		rc = sqlite3_open("pacman.db", &db);
		if(rc){
			const char* zErrMsg = sqlite3_errmsg(db);
			sqlite3_close(db);
			throw PacmanDBException(zErrMsg);
		}
		std::string sql = (boost::format("UPDATE KeyBindings SET Key = %1% WHERE Name = '%2%'")%keyvalue%name).str();

		const char *s = NULL;
		rc = sqlite3_prepare(db, sql.c_str(), sql.size(), &res, &s);
		if (rc != SQLITE_OK)
		{
			return;
		}
		if (!res)
		{
			return;
		}
		rc = sqlite3_step(res); // execute
		sqlite3_finalize(res); // deallocate statement
		res = NULL;
		switch (rc)
		{
		case SQLITE_BUSY:
			return;
		case SQLITE_DONE:
		case SQLITE_ROW:
			return;
		case SQLITE_ERROR:
			return;
		case SQLITE_MISUSE:
			return;
		}
		if(rc!=SQLITE_OK){
			throw PacmanDBException("Error in PacmanDB.");
		}
		sqlite3_close(db);
	}

	std::multimap<std::string, std::string> GetKeyUpBindings(){
		std::multimap<std::string, std::string> keyupbindings;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT Name, Event FROM KeyUpBindings");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int nameidx, eventidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Name"){
				nameidx = idx;
			}
			else if(*itr=="Event"){
				eventidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			std::string name = *(itr+nameidx);
			std::string event = *(itr+eventidx);

			keyupbindings.insert(std::multimap<std::string,std::string>::value_type(name, event));
		}

		return keyupbindings;
	}

	std::multimap<std::string, std::string> GetKeyDownBindings(){
		std::multimap<std::string, std::string> keydownbindings;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT Name, Event FROM KeyDownBindings");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int nameidx, eventidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Name"){
				nameidx = idx;
			}
			else if(*itr=="Event"){
				eventidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			std::string name = *(itr+nameidx);
			std::string event = *(itr+eventidx);

			keydownbindings.insert(std::multimap<std::string,std::string>::value_type(name, event));
		}

		return keydownbindings;
	}

	std::multimap<std::string, std::string> GetKeyHeldBindings(){
		std::multimap<std::string, std::string> keyheldbindings;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT Name, Event FROM KeyHeldBindings");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int nameidx, eventidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Name"){
				nameidx = idx;
			}
			else if(*itr=="Event"){
				eventidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			std::string name = *(itr+nameidx);
			std::string event = *(itr+eventidx);

			keyheldbindings.insert(std::multimap<std::string,std::string>::value_type(name, event));
		}

		return keyheldbindings;
	}

	std::vector<SoundBinding> GetSoundBindings(){
		std::vector<SoundBinding> soundbindings;

		char* zErrMsg = 0;
		char** result ;
		int rc = 0;
		int nrow,ncol;
		std::vector<std::string> vcol_head;
		std::vector<std::string> vdata;


		const std::string sql("SELECT Event, Filename FROM SoundBindings");

		rc = sqlite3_get_table(
			db,              /* An open database */
			sql.c_str(),       /* SQL to be executed */
			&result,       /* Result written to a char *[]  that this points to */
			&nrow,             /* Number of result rows written here */
			&ncol,          /* Number of result columns written here */
			&zErrMsg          /* Error msg written here */
			);


		if( rc == SQLITE_OK ){
			for(int i=0; i < ncol; ++i){
				vcol_head.push_back(result[i]);   /* First row heading */
			}
			for(int i=0; i < ncol*nrow; ++i){
				vdata.push_back(result[ncol+i]);
			}
		}
		sqlite3_free_table(result);


		int eventidx, filenameidx;

		int idx=0;
		for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
			if(*itr=="Event"){
				eventidx = idx;
			}
			else if(*itr=="Filename"){
				filenameidx = idx;
			}
			idx++;
		}

		for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
			std::string event = *(itr+eventidx);
			std::string filename = *(itr+filenameidx);

			soundbindings.push_back(SoundBinding(event, filename));
		}

		return soundbindings;
	}
	std::vector<MusicBinding> GetMusicBindings(){
			std::vector<MusicBinding> musicbindings;

			char* zErrMsg = 0;
			char** result ;
			int rc = 0;
			int nrow,ncol;
			std::vector<std::string> vcol_head;
			std::vector<std::string> vdata;


			const std::string sql("SELECT Event, Filename, FadeInTime, FadeOutTime FROM MusicBindings");

			rc = sqlite3_get_table(
				db,              /* An open database */
				sql.c_str(),       /* SQL to be executed */
				&result,       /* Result written to a char *[]  that this points to */
				&nrow,             /* Number of result rows written here */
				&ncol,          /* Number of result columns written here */
				&zErrMsg          /* Error msg written here */
				);


			if( rc == SQLITE_OK ){
				for(int i=0; i < ncol; ++i){
					vcol_head.push_back(result[i]);   /* First row heading */
				}
				for(int i=0; i < ncol*nrow; ++i){
					vdata.push_back(result[ncol+i]);
				}
			}
			sqlite3_free_table(result);


			int eventidx, filenameidx, fadeintimeidx, fadeouttimeidx;

			int idx=0;
			for(std::vector<std::string>::const_iterator itr = vcol_head.begin(); itr!=vcol_head.end();itr++){
				if(*itr=="Event"){
					eventidx = idx;
				}
				else if(*itr=="Filename"){
					filenameidx = idx;
				}
				else if(*itr=="FadeInTime"){
					fadeintimeidx = idx;
				}
				else if(*itr=="FadeOutTime"){
					fadeouttimeidx = idx;
				}
				idx++;
			}

			for(std::vector<std::string>::const_iterator itr = vdata.begin(); itr!=vdata.end(); itr+=vcol_head.size()){
				std::string event = *(itr+eventidx);
				std::string filename = *(itr+filenameidx);
				int fadeintime = boost::lexical_cast<int>(*(itr+fadeintimeidx));
				int fadeouttime = boost::lexical_cast<int>(*(itr+fadeouttimeidx));

				musicbindings.push_back(MusicBinding(event, filename, fadeintime, fadeouttime));
			}

			return musicbindings;
		}
};

#endif /* PACMANDB_HPP_ */
