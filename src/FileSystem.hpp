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
#define PACMAN_DEBUG
#ifdef PACMAN_DEBUG
	std::string FileSystem::usrlocal = ".";
	std::string FileSystem::vargames = ".";
#else
	std::string FileSystem::usrlocal = "/usr/local/games/pacman";
	std::string FileSystem::vargames = "/var/games/pacman";
#endif


#endif// FILESYSTEM_HPP_
