/*
Copyright 2012 JohannC

This file is part of TITRUS.

TITRUS is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

TITRUS is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
TITRUS.  If not, see http://www.gnu.org/licenses/
*/

#include <utility/Configuration.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif


using namespace utility;

const std::string Configuration::separator="/";

Configuration::Configuration(const std::string & _name): name(_name)
{
    home.assign(getenv("HOME"));
    home.append(separator); home.append(".poufpouf");

    serverId    = 0;
    touch       = 2;
    audio       = 2;
    handicap    = 0;
    scoreNames[0] = "JOH";          scoreValues[0] = 1500;
    scoreNames[1] = "MAD";          scoreValues[1] = 1200;
    scoreNames[2] = "CHA";          scoreValues[2] = 1000;
    scoreNames[3] = "BEN";          scoreValues[3] = 900;
    scoreNames[4] = "VAL";          scoreValues[4] = 800;
    scoreNames[5] = "MAR";          scoreValues[5] = 700;
    scoreNames[6] = "FRE";          scoreValues[6] = 600;
    scoreNames[7] = "CAR";          scoreValues[7] = 500;
    scoreNames[8] = "VIN";          scoreValues[8] = 400;
    scoreNames[9] = "LAU";          scoreValues[9] = 250;
}


/**
 * Check to see if a directory/folder exists
 */
bool Configuration::dirExists(const std::string& _path)
{
    struct stat st;
    return (stat(_path.c_str(), &st) == 0);
}

/**
 * Create this folder if it doesn't already exist
 */
void Configuration::createDir(const std::string& _path)
{

#ifndef _WIN32
    mkdir(_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif

#ifdef _WIN32
    std::string syscmd = "mkdir " + _path;
    system(syscmd.c_str());
#endif
}

/**
 * Check to see if a file exists
 * The filename parameter should include the entire path to this file
 */
bool Configuration::fileExists(const std::string& _filename)
{
    std::ifstream infile(_filename.c_str());
    return infile;
}

/**
 * Load the configuration file
 */
void Configuration::load()
{
    // HANDLE THE HOME FOLDER
    if (!dirExists(home))
    {
        std::cout<<"Create new configuration folder: "<<home<<std::endl;
        createDir(home);
    }

    // BUILD THE FILENAME
    std::string filename(home);
    filename.append(separator);
    filename.append(name);
    std::cout<<"Read "<<filename<<std::endl;

    bool rc = false;

    // READ THE SETTINGS CONFIGURATION FILE
    if (fileExists(filename))
    {
        try { config.readFile(filename.c_str()); rc = true; }
        catch(libconfig::FileIOException e) {}
        catch(libconfig::ParseException  e) {}
    }

    // IF FAILED, CREATE A DEFAULT CONFIG
    if (!rc)
    {
        initConfig(config.getRoot());
    }
}

/**
 * Create a generic default configuration
 * @param _root is the configuration to fill
 */
void Configuration::initConfig(libconfig::Setting & _root)
{
    try
    {
        // GENERIC SCORE
        libconfig::Setting& score =  _root.add("score", libconfig::Setting::TypeList);
        for (int i=0; i<10; i++)
        {
            libconfig::Setting& s = score.add(libconfig::Setting::TypeGroup);
            s.add("player", libconfig::Setting::TypeString) = scoreNames[i];
            s.add("score", libconfig::Setting::TypeInt) = scoreValues[i];
        }

        // GENERIC PARAMETER
        _root.add("server", libconfig::Setting::TypeInt)    = serverId;
        _root.add("audio", libconfig::Setting::TypeInt)     = audio;
        _root.add("touch", libconfig::Setting::TypeInt)     = touch;
        _root.add("handicap", libconfig::Setting::TypeInt)  = handicap;

    }
    catch(libconfig::SettingTypeException  e) { std::cout<<"SettingTypeException"<<std::endl; }
}

/**
 * Save the configuration file
 */
void Configuration::save()
{
    if (dirExists(home))
    {
        // BUILD THE FILENAME
        std::string filename(home);
        filename.append(separator);
        filename.append(name);
        std::cout<<"Save "<<filename<<std::endl;

        try { config.writeFile(filename.c_str()); }
        catch(libconfig::FileIOException e) {}
    }
}


