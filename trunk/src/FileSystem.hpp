/*
 * FileSystem.hpp
 *
 *  Created on: Feb 21, 2010
 *      Author: aaronsantos
 */


#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

class FileSystem{
public:
	static std::string usrlocal;
	static std::string vargames;
	static std::string MakeUsrLocalPath(const std::string& relativepath){
		return usrlocal+relativepath;
	}
	static std::string MakeVarGamesPath(const std::string& relativepath){
		return vargames+relativepath;
	}
};
#define TETRIS_DEBUG
#ifdef TETRIS_DEBUG
	std::string FileSystem::usrlocal = std::string(".");
	std::string FileSystem::vargames = std::string(".");
#else
	std::string FileSystem::usrlocal = std::string("/usr/local/games/pacman");
	std::string FileSystem::vargames = std::string("/var/games/pacman");
#endif


#endif// FILESYSTEM_HPP_
