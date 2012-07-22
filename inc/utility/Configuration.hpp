/*
Copyright 2011 JohannC

This file is part of TOMATOES.

TOMATOES is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

TOMATOES is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
TOMATOES.  If not, see http://www.gnu.org/licenses/
*/

#ifndef UTILITY_CONFIGURATION_HPP_
#define UTILITY_CONFIGURATION_HPP_

#include <libconfig.h++>
#include <string>

namespace utility
{

class Configuration
{
private:
    libconfig::Config           config;
    static const std::string    separator;
    std::string                 name;
    std::string                 home;

public:
    unsigned short              serverId;
    std::string                 scoreNames[10];
    unsigned short              scoreValues[10];
    unsigned short              touch;
    unsigned short              audio;
    unsigned short              handicap;

private:
    /**
     * Check to see if a directory/folder exists
     */
    bool dirExists(const std::string& _path);

    /**
     * Create this folder if it doesn't already exist
     */
    void createDir(const std::string& path);

    /**
     * Check to see if a file exists
     * The filename parameter should include the entire path to this file
     */
    bool fileExists(const std::string& _filename);

protected:
    /**
     * Create a generic default configuration
     * @param _root is the configuration to fill
     */
    virtual void initConfig(libconfig::Setting & _root);

public:
    Configuration(const std::string & _name);

    /**
     * Load the configuration file
     */
    virtual void load();

    /**
     * Save the configuration file
     */
    virtual void save();

    /**
     * get the setting
     */
    libconfig::Config & getConfig() { return config; }

    /**
     * get the root
     */
    libconfig::Setting& getRoot() { return config.getRoot(); }
};


}

#endif
