/*
Copyright 2012 JohannC

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

#include <splashouille/Defines.hpp>
#include <splashouille/Engine.hpp>
#include <splashouille/Library.hpp>
#include <splashouille/Style.hpp>
#include <splashouille/Fashion.hpp>
#include <splashouille/Crowd.hpp>
#include <splashouille/Sound.hpp>
#include <splashouille/Image.hpp>
#include <splashouille/Animation.hpp>

#include <tomatoes/Main.hpp>
#include <tomatoes/Game.hpp>

#include <iostream>
#include <sstream>

using namespace tomatoes;

/** The 3 airships models */
const Game::Player::Model  Game::Player::models[Game::nbModels] = {
        Game::Player::Model(1000,200,300,10,trident,4,20,20,"_gnu"),
        Game::Player::Model(700,200,200,20,laser,5,40,20,"_tux"),
        Game::Player::Model(1500,500,350,5,gunshot,3,30,20,"_fox") };

/** The delay between two fires regarding the fire type and the player level */
const int Game::Player::delayFire[Game::Player::lastfire][Game::Player::nbMaxLevels] = {
        { 100, 65, 40}, { 100, 100, 100}, { 100, 100, 100}  };

/**
 * Init a player
 */
void Game::Player::init(Game * _parent, int _ts, int _playerId, int _model, bool _multi)
{
    if ((_playerId==0) || ((_playerId==1)&&(_multi)))
    {
        parent = _parent;
        model = _model;
        life = models[model].life;
        char base[128];     snprintf(base, 128, "%s%02d", Game::Player::models[model].name.c_str(), (_playerId+1));
        char name[128];     snprintf(name, 128, "player%d", (_playerId+1));
        char fashion[128];  snprintf(fashion, 128, "intro%d", _multi?2:1);
        object = _parent->engine->getLibrary()->copyObject(base, name);
        object->changeFashion(fashion);

        splashouille::Animation * anim = _parent->engine->getLibrary()->getAnimationById("a_players");
        anim->getCrowd()->insertObject(_ts, object);
    }
}

/**
 * Get the player position
 */
void Game::Player::getPosition(float &_x, float &_y)
{
    _x = object->getStyle()->getLeft()+60;
    _y = object->getStyle()->getTop()+60; 
}

/**
 * Update the speed
 */
void Game::Player::addSpeed(float _x, float _y)
{
    speed[0] += _x;
    speed[1] += _y;
}


/**
 * Update the player position
 */
void Game::Player::update(int _ts, bool _up, bool _right, bool _down, bool _left, bool _fire)
{
    if (timestamp && (life>0) )
    {
        float dt = (float)(_ts - timestamp)/1000;

        // DECELERATE
        for (int i=0; i<2; i++)
        {
            if (speed[i] > dt*models[model].deceleration) { speed[i] -= dt*models[model].deceleration; } else
            if (speed[i] <-dt*models[model].deceleration) { speed[i] += dt*models[model].deceleration; } else
            speed[i] = 0;
        }

        // ACCELERATE
        if (_up&&_left)     { speed[0] -= dt*models[model].acceleration/1.44; speed[1] -= dt*models[model].acceleration/1.44; } else
        if (_up&&_right)    { speed[0] += dt*models[model].acceleration/1.44; speed[1] -= dt*models[model].acceleration/1.44; } else
        if (_down&&_right)  { speed[0] += dt*models[model].acceleration/1.44; speed[1] += dt*models[model].acceleration/1.44; } else
        if (_down&&_left)   { speed[0] -= dt*models[model].acceleration/1.44; speed[1] += dt*models[model].acceleration/1.44; } else
        if (_up)            { speed[1] -= dt*models[model].acceleration; } else
        if (_right)         { speed[0] += dt*models[model].acceleration; } else
        if (_down)          { speed[1] += dt*models[model].acceleration; } else
        if (_left)          { speed[0] -= dt*models[model].acceleration; }

        // CROP
        for (int i=0; i<2; i++)
        {
            if (speed[i] > models[model].maxSpeed) speed[i] = models[model].maxSpeed; else
            if (speed[i] <-models[model].maxSpeed) speed[i] =-models[model].maxSpeed;
        }

        // UPDATE POSITION
        float left  = object->getStyle()->getLeft()+speed[0]*dt;
        float top   = object->getStyle()->getTop()+speed[1]*dt;
        if (left<-models[model].offset[0])      { left = -models[model].offset[0]; } else
        if (left>360+models[model].offset[0])   { left = 360+models[model].offset[0]; }
        if (top<-models[model].offset[1])       { top = -models[model].offset[1]; } else
        if (top>520+models[model].offset[1])    { top = 520 + models[model].offset[1]; }
        object->getStyle()->setLeft(left);
        object->getStyle()->setTop(top);

        // FASHION
        float thr = 100;
        if (speed[0]<-thr && speed[1]<-thr)     { object->changeFashion("leftup"); } else
        if (speed[0]>+thr && speed[1]<-thr)     { object->changeFashion("rightup"); } else
        if (speed[0]>+thr && speed[1]>+thr)     { object->changeFashion("rightdown"); } else
        if (speed[0]<-thr && speed[1]>+thr)     { object->changeFashion("leftdown"); } else
        if (speed[1]<-thr*1.44)                 { object->changeFashion("up"); } else
        if (speed[0]>+thr*1.44)                 { object->changeFashion("right"); } else
        if (speed[1]>+thr*1.44)                 { object->changeFashion("down"); } else
        if (speed[0]<-thr*1.44)                 { object->changeFashion("left"); } else
        object->changeFashion("center");

        // FIRE SOME BULLETS ?
        if (_fire && _ts-lastBulletTs>delayFire[models[model].fireType][level])
        {
            parent->fire(_ts, left,top, models[model].fireType, level);
            lastBulletTs = _ts;
        }
    }
    timestamp = _ts;
}

/**
 *
 */
bool Game::Player::hit(int _ts, int _value)
{
    bool ret = (_ts-lastHit>200);
    if (ret)
    {
        life -= _value;
        if (life<=0)
        {
            object->changeFashion("outro");
        }
        lastHit = _ts;
    }
    return ret;
}

/**
 * Initialize the game level
 * @param _level is the level id
 */
void Game::init(int _level)
{
    bool rc = true;

    // CREATE AND CONFIGURE THE ENGINE
    engine = splashouille::Engine::createEngine();

    libconfig::Config * configuration   = new libconfig::Config();

    try { configuration->readFile("res/conf/levels/level01.ini"); }
        catch(libconfig::FileIOException e) { std::cerr<<e.what()<<std::endl; rc = false; }
        catch(libconfig::ParseException  e) { std::cerr<<e.what()<<std::endl; rc = false; }

    if (rc) { rc = engine->import(configuration, true); }
}

/**
 * Initialize the menu  handler
 * @return true if every objects are found (todo)
 */
void Game::run(SDL_Surface * _screen)
{
    // GET ELEMENTS
    a_shoots            = engine->getLibrary()->getAnimationById("a_shoots");
    a_ennemies          = engine->getLibrary()->getAnimationById("a_ennemies");
    a_bullets           = engine->getLibrary()->getAnimationById("a_bullets");
    a_effects           = engine->getLibrary()->getAnimationById("a_effects");

    // HANDLE CALLBACKS
    for (int i=0; i<last; i++) { onFrameMethods[i] = 0; }
    onFrameMethods[level1]      = &Game::onLevel1;
    onFrameMethods[level2]      = &Game::onLevel2;

    // HANDLE FIRE
    for (int i=0; i<Player::lastfire*100; i++) { fireMethods[i] = 0; }
    fireMethods[getFireId(Player::trident, 0)]  = &Game::trident0;
    fireMethods[getFireId(Player::trident, 1)]  = &Game::trident1;
    fireMethods[getFireId(Player::trident, 2)]  = &Game::trident2;
    fireMethods[getFireId(Player::gunshot, 0)]  = &Game::gunshot0;
    fireMethods[getFireId(Player::gunshot, 1)]  = &Game::gunshot1;
    fireMethods[getFireId(Player::gunshot, 2)]  = &Game::gunshot2;
    fireMethods[getFireId(Player::laser, 0)]    = &Game::laser0;
    fireMethods[getFireId(Player::laser, 1)]    = &Game::laser1;
    fireMethods[getFireId(Player::laser, 2)]    = &Game::laser2;

    // HANDLE BULLET
    for (int i=0; i<lastbullet; i++) { bulletMethods[i] = 0; }
    bulletMethods[simple]       = &Game::bulletSimple;
    bulletMethods[duo]          = &Game::bulletDuo;
    bulletMethods[duosw]        = &Game::bulletDuoSW;
    bulletMethods[duose]        = &Game::bulletDuoSE;
    bulletMethods[cross4]       = &Game::bulletCross4;
    bulletMethods[cross8]       = &Game::bulletCross8;
    bulletMethods[cross16]      = &Game::bulletCross16;
    bulletMethods[cross16bis]   = &Game::bulletCross16Bis;
    bulletMethods[cross32]      = &Game::bulletCross32;
    bulletMethods[dir00]        = &Game::bulletDir00;
    bulletMethods[dir01]        = &Game::bulletDir01;
    bulletMethods[dir02]        = &Game::bulletDir02;
    bulletMethods[dir03]        = &Game::bulletDir03;
    bulletMethods[dir04]        = &Game::bulletDir04;
    bulletMethods[dir05]        = &Game::bulletDir05;
    bulletMethods[dir06]        = &Game::bulletDir06;
    bulletMethods[dir07]        = &Game::bulletDir07;
    bulletMethods[dir08]        = &Game::bulletDir08;
    bulletMethods[dir09]        = &Game::bulletDir09;
    bulletMethods[dir10]        = &Game::bulletDir10;
    bulletMethods[dir11]        = &Game::bulletDir11;
    bulletMethods[dir12]        = &Game::bulletDir12;
    bulletMethods[dir13]        = &Game::bulletDir13;
    bulletMethods[dir14]        = &Game::bulletDir14;
    bulletMethods[dir15]        = &Game::bulletDir15;
    bulletMethods[dir16]        = &Game::bulletDir16;
    bulletMethods[dir17]        = &Game::bulletDir17;
    bulletMethods[dir18]        = &Game::bulletDir18;
    bulletMethods[dir19]        = &Game::bulletDir19;
    bulletMethods[dir20]        = &Game::bulletDir20;
    bulletMethods[dir21]        = &Game::bulletDir21;
    bulletMethods[dir22]        = &Game::bulletDir22;
    bulletMethods[dir23]        = &Game::bulletDir23;
    bulletMethods[dir24]        = &Game::bulletDir24;
    bulletMethods[dir25]        = &Game::bulletDir25;
    bulletMethods[dir26]        = &Game::bulletDir26;
    bulletMethods[dir27]        = &Game::bulletDir27;
    bulletMethods[dir28]        = &Game::bulletDir28;
    bulletMethods[dir29]        = &Game::bulletDir29;
    bulletMethods[dir30]        = &Game::bulletDir30;
    bulletMethods[dir31]        = &Game::bulletDir31;
    bulletMethods[midcross8]    = &Game::bulletMidCross8;
    bulletMethods[midcross16]   = &Game::bulletMidCross16;

    engine->addListener(this);
    engine->setFPS(600);

    // RUN THE ENGINE
    if (engine) { engine->run(_screen); }

    // DELETE THE ENGINE
    splashouille::Engine::deleteEngine(engine);
}


void Game::fire(int _ts, float _left, float _top, Player::fire _fireType, int _level)
{
    p_fire  method = fireMethods[getFireId(_fireType, _level)];
    if (method) { (*this.*method)(_ts, _left, _top); }
}


/**
 * Callback called each second
 * @param _frame is the frame number from the beginning of the animation
 * @param _frameSec is the number of frames played during the last second
 * @param _second is the current second
 */
void Game::onSecond(int _frame, int _frameSec, int _second)
{
std::cout<<"["<<_frame<<"|"<<_second<<"] (fps:"<<_frameSec<<") ("<<a_shoots->getCrowd()->getSize()<<"|"<<
    a_ennemies->getCrowd()->getSize()<<"|"<<a_bullets->getCrowd()->getSize()<<"|"<<
    a_effects->getCrowd()->getSize()<<") ("<<engine->getLibrary()->getSize()<<")"<<std::endl;
}

/**
 * Callback called on each frame
 * @param _frame is the frame number from the beginning of the animation
 * @param _timeStampInMilliSeconds is the current timestamp
 */
void Game::onFrame(int _frame, int _timeStampInMilliSeconds)
{
    p_onFrame  method = onFrameMethods[engine->getState()];
    if (method) { (*this.*method)(_frame, _timeStampInMilliSeconds); }
}

/**
 * The onEvent Callback
 * @param _event is the SDL_Event
 */
bool Game::onEvent(SDL_Event & _event, int _timeStampInMilliSeconds UNUSED)
{
    tomatoes->getKeyboard().onEvent(_event);
    return true;
}


/**
 * Handle the level1 event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Game::onLevel1(int _frame UNUSED, int _timeStampInMilliSeconds)
{
    players[0].init(this, _timeStampInMilliSeconds, 0, tomatoes->getPlayerId(0), tomatoes->multi());
    players[1].init(this, _timeStampInMilliSeconds, 1, tomatoes->getPlayerId(1), tomatoes->multi());

    engine->setState(0);
}

/**
 * Handle the level1 event
 * @param _event
 * @param _timeStampInMilliSeconds is the unused timestamp
 * @return true
 */
void Game::onLevel2(int _frame UNUSED, int _timeStampInMilliSeconds)
{
    // SAVE THE TIMESTAMP
    currentTs = _timeStampInMilliSeconds;

    // PLAYERS BULLETS TREATMENT
    for (int i=0; i<10; i++) for (int j=0; j<10; j++) { alliesArray[i][j].clear(); }
    alliesToDelete.clear();
    a_shoots->getCrowd()->forEach(this, "", true, allies);

    // TOMATOES UPDATE
    ennemiesToDelete.clear();
    a_ennemies->getCrowd()->forEach(this, "", true, ennemies);

    // ENNEMIES BULLETS UPDATE
    bulletsToDelete.clear();
    a_bullets->getCrowd()->forEach(this, "", true, bullets);

    // EFFECTS UPDATE
    effectsToDelete.clear();
    a_effects->getCrowd()->forEach(this, "", true, effects);

    // DELETE OBJECTS
    while (!alliesToDelete.empty())
    {
        splashouille::Object * object = alliesToDelete.front();
        a_shoots->getCrowd()->dropObject(object);
        engine->getLibrary()->deleteObject(object);
        alliesToDelete.pop_front();
    }
    while (!ennemiesToDelete.empty())
    {
        splashouille::Object * object = ennemiesToDelete.front();
        a_ennemies->getCrowd()->dropObject(object);
        engine->getLibrary()->deleteObject(object);
        ennemiesToDelete.pop_front();
    }
    while (!bulletsToDelete.empty())
    {
        splashouille::Object * object = bulletsToDelete.front();
        a_bullets->getCrowd()->dropObject(object);
        engine->getLibrary()->deleteObject(object);
        bulletsToDelete.pop_front();
    }
    while (!effectsToDelete.empty())
    {
        splashouille::Object * object = effectsToDelete.front();
        a_effects->getCrowd()->dropObject(object);
        engine->getLibrary()->deleteObject(object);
        effectsToDelete.pop_front();
    }

    // PLAYER KEYBOARD INTERACTION
    Uint8 * keystates = tomatoes->getKeyboard().SDL_GetKeyStateLight(NULL);
    players[0].update(  _timeStampInMilliSeconds,
                        keystates[SDLK_UP], keystates[SDLK_RIGHT],keystates[SDLK_DOWN],keystates[SDLK_LEFT],
                        keystates[' '] | keystates['l'] );

    if (tomatoes->multi())
    {
        players[1].update(  _timeStampInMilliSeconds,
                            keystates['r'], keystates['g'],keystates['f'],keystates['d'],
                            keystates['e'] );

        // PLAYERS REBOUND
        float p1[2], p2[2];
        players[0].getPosition(p1[0], p1[1]);
        players[1].getPosition(p2[0], p2[1]);
        if ( (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) < 1024)
        {
            players[0].addSpeed((p1[0]-p2[0])*(25-players[0].getModel().weight), (p1[1]-p2[1])*(25-players[0].getModel().weight));
            players[1].addSpeed((p2[0]-p1[0])*(25-players[1].getModel().weight), (p2[1]-p1[1])*(25-players[1].getModel().weight));
        }
    }
}


/** Crowd callback */
bool Game::onObject(splashouille::Object * _object, int _user)
{
    if (_user==allies)
    {
        float top = _object->getFashion()->getCurrent()->getTop()+_object->getFashion()->getCurrent()->getRelativeTop();
        float left= _object->getFashion()->getCurrent()->getLeft()+_object->getFashion()->getCurrent()->getRelativeLeft();
        float width = _object->getFashion()->getCurrent()->getWidth();
        float height = _object->getFashion()->getCurrent()->getHeight();

        if ((top<-20) || (top>640) || (left<-20) || (left>480) || (_object->getState()==0) )
        {
            alliesToDelete.push_back(_object);
        }
        else
        {
            int imin = left/48;         if (imin<0) { imin = 0; } if (imin>9) { imin = 9; }
            int imax = (left+width)/48; if (imax<0) { imax = 0; } if (imax>9) { imax = 9; }
            int jmin = top/64;          if (jmin<0) { jmin = 0; } if (jmin>9) { jmin = 9; }
            int jmax = (top+height)/64; if (jmax<0) { jmax = 0; } if (jmax>9) { jmax = 9; }

            for (int i=imin; i<=imax; i++) for (int j=jmin; j<=jmax; j++) {alliesArray[i][j].push_back(_object); }
        }
    }
    else
    if (_user==ennemies)
    {
        float top = _object->getFashion()->getCurrent()->getTop()+_object->getFashion()->getCurrent()->getRelativeTop();
        float left= _object->getFashion()->getCurrent()->getLeft()+_object->getFashion()->getCurrent()->getRelativeLeft();
        float width = _object->getFashion()->getCurrent()->getWidth();
        float height = _object->getFashion()->getCurrent()->getHeight();

        if (top>640)
        {
            ennemiesToDelete.push_back(_object);
        }
        else
        {
            // SEND BULLET
            p_bullet  method = bulletMethods[_object->getStyle()->getUser()];
            if (method) { (*this.*method)(currentTs, left+(width/2), top+(height/2)); }
            _object->getStyle()->setUser(0);

            // COLLIZIONS WITH SHOOT
            int imin = left/48;         if (imin<0) { imin = 0; } if (imin>9) { imin = 9; }
            int imax = (left+width)/48; if (imax<0) { imax = 0; } if (imax>9) { imax = 9; }
            int jmin = top/64;          if (jmin<0) { jmin = 0; } if (jmin>9) { jmin = 9; }
            int jmax = (top+height)/64; if (jmax<0) { jmax = 0; } if (jmax>9) { jmax = 9; }
            top         += height/2;
            left        += width/2;

            int life = _object->getState()%1000;
            for (int i=imin; i<=imax; i++) for (int j=jmin; j<=jmax; j++)
            for (std::list<splashouille::Object*>::iterator it = alliesArray[i][j].begin(); it != alliesArray[i][j].end(); it++)
            {
                if (life>0)
                {
                    splashouille::Object * shoot = *it;
                    float topShoot = shoot->getFashion()->getCurrent()->getTop()+shoot->getFashion()->getCurrent()->getRelativeTop();
                    float leftShoot= shoot->getFashion()->getCurrent()->getLeft()+shoot->getFashion()->getCurrent()->getRelativeLeft();
                    float widthShoot = shoot->getFashion()->getCurrent()->getWidth();
                    topShoot    += widthShoot/2;
                    leftShoot   += widthShoot/2;

                    float radius2 = (width/2)*(width/2) + (widthShoot/2)*(widthShoot/2);
                    float dist2 = (top-topShoot)*(top-topShoot) + (left-leftShoot)*(left-leftShoot);

                    if (shoot->getState()>0 && (dist2<radius2))
                    {
                        // HANDLE THE SHOOT
                        int loss = shoot->getState();
                        if (loss>life) { loss = life; }
                        life -= loss;
                        shoot->setState(0);
                        _object->setState(_object->getState() - loss);
                        alliesToDelete.push_back(shoot);

                        // DESTROY THE ENNEMY AND ADD THE EXPLOSION ANIMATION
                        if (life<=0)
                        {
                            ennemiesToDelete.push_back(_object);
                            char name[128]; snprintf(name, 128, "effect%05d", bulletId);
                            bulletId = (bulletId+1)%100000;
                            splashouille::Object * object = engine->getLibrary()->copyObject("_explosion", name);
                            object->getStyle()->setTop(top - object->getStyle()->getHeight()/2);
                            object->getStyle()->setLeft(left - object->getStyle()->getWidth()/2);
                            a_effects->getCrowd()->insertObject(currentTs, object);

                            // EVENT
                            int event = (_object->getState()/1000)%10;
                            if (event == 1)
                            {
                                char name[128]; snprintf(name, 128, "effect%05d", bulletId);
                                bulletId = (bulletId+1)%100000;
                                splashouille::Object * object = engine->getLibrary()->copyObject("_powerup", name);
                                object->getStyle()->setTop(topShoot - object->getStyle()->getHeight()/2 - 10);
                                object->getStyle()->setLeft(leftShoot - object->getStyle()->getWidth()/2);
                                object->setState(1);
                                a_effects->getCrowd()->insertObject(currentTs, object);
                            }
                            else
                            if (event == 5)
                            {
                                bulletCross32(currentTs, left, top);
                            }
                        }
                    }
                }
            }

            // COLLIZIONS WITH PLAYERS
            for (int i=0; i<(tomatoes->multi()?2:1); i++) if (players[i].isAlive())
            {
                float p[2]; players[i].getPosition(p[0], p[1]);

                float radius2 = (width/2)*(width/2) + (64);
                float dist2 = (top-p[1])*(top-p[1]) + (left-p[0])*(left-p[0]);
                if (dist2<radius2)
                {
                    _object->setState(0);

                    ennemiesToDelete.push_back(_object);
                    char name[128]; snprintf(name, 128, "effect%05d", bulletId);
                    bulletId = (bulletId+1)%100000;
                    splashouille::Object * object = engine->getLibrary()->copyObject("_explosion", name);
                    object->getStyle()->setTop(top - object->getStyle()->getHeight()/2);
                    object->getStyle()->setLeft(left - object->getStyle()->getWidth()/2);
                    a_effects->getCrowd()->insertObject(currentTs, object);

                    if (players[i].hit(currentTs, 100))
                    {
                        name[128]; snprintf(name, 128, "effect%05d", bulletId);
                        bulletId = (bulletId+1)%100000;
                        splashouille::Object * object = engine->getLibrary()->copyObject("_bam", name);
                        object->getStyle()->setTop(p[1] - object->getStyle()->getHeight()/2 - 10);
                        object->getStyle()->setLeft(p[0] - object->getStyle()->getWidth()/2);
                        a_effects->getCrowd()->insertObject(currentTs, object);
                    }
                }
            }
        }
    }
    else
    if (_user==bullets)
    {
        float top = _object->getFashion()->getCurrent()->getTop()+_object->getFashion()->getCurrent()->getRelativeTop();
        float left= _object->getFashion()->getCurrent()->getLeft()+_object->getFashion()->getCurrent()->getRelativeLeft();
        float width = _object->getFashion()->getCurrent()->getWidth();
        float height = _object->getFashion()->getCurrent()->getHeight();

        if ((top<-height) || (top>640) || (left<-width) || (left>480) || (_object->getState()==0) )
        {
            bulletsToDelete.push_back(_object);
        }
        else
        {
            // CHECK COLLISION WITH PLAYER
            for (int i=0; i<(tomatoes->multi()?2:1); i++) if (players[i].isAlive())
            {
                top+=height/2; left+=width/2;
                float p[2]; players[i].getPosition(p[0], p[1]);

                float radius2 = (width/2)*(width/2) + (64);
                float dist2 = (top-p[1])*(top-p[1]) + (left-p[0])*(left-p[0]);
                if (dist2<radius2)
                {
                    bulletsToDelete.push_back(_object);
                    if (players[i].hit(currentTs, 1))
                    {
                        char name[128]; snprintf(name, 128, "effect%05d", bulletId);
                        bulletId = (bulletId+1)%100000;
                        splashouille::Object * object = engine->getLibrary()->copyObject("_bam", name);
                        object->getStyle()->setTop(p[1] - object->getStyle()->getHeight()/2);
                        object->getStyle()->setLeft(p[0] - object->getStyle()->getWidth()/2);
                        a_effects->getCrowd()->insertObject(currentTs, object);
                    }
                }
            }
        }
    }
    else
    if (_user==effects)
    {
        if (!_object->getStyle()->getDisplay()) { effectsToDelete.push_back(_object); }
        else
        // POWERUP
        if (_object->getState()==1)
        {
            float top = _object->getFashion()->getCurrent()->getTop()+_object->getFashion()->getCurrent()->getRelativeTop();
            float left= _object->getFashion()->getCurrent()->getLeft()+_object->getFashion()->getCurrent()->getRelativeLeft();
            float width = _object->getFashion()->getCurrent()->getWidth();
            float height = _object->getFashion()->getCurrent()->getHeight();

            // CHECK COLLISION WITH PLAYER
            for (int i=0; i<(tomatoes->multi()?2:1); i++) if (players[i].isAlive())
            {
                top+=height/2; left+=width/2;
                float p[2]; players[i].getPosition(p[0], p[1]);

                float radius2 = (width/2)*(width/2) + (256);
                float dist2 = (top-p[1])*(top-p[1]) + (left-p[0])*(left-p[0]);
                if (dist2<radius2)
                {
                    _object->getStyle()->setDisplay(false);
                    players[i].levelUp();
                }
            }
        }
    }
    return true;
}


void Game::bullet01 (int _ts, float _left, float _top, const std::string & _fashion, int _positionY, int _power)
{
    char name[128]; snprintf(name, 128, "bullet%05d", bulletId);
    bulletId = (bulletId+1)%100000;
    splashouille::Object * object = engine->getLibrary()->copyObject("_bullet", name);
    object->getStyle()->setTop(_top);
    object->getStyle()->setLeft(_left);
    object->getStyle()->setPositionY(_positionY);
    object->changeFashion(_fashion);
    object->setState(_power);
    a_shoots->getCrowd()->insertObject(_ts, object);
}

void Game::trident0 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+50, _top+20, "n2", 32);
}

void Game::trident1 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+65, _top+20, "n2", 32);
    bullet01(_ts, _left+35, _top+20, "n2", 32);
}

void Game::trident2 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+65, _top+20, "n2", 32);
    bullet01(_ts, _left+35, _top+20, "n2", 32);
    bullet01(_ts, _left+70, _top+20, "nne2", 32);
    bullet01(_ts, _left+30, _top+20, "nnw2", 32);
}

void Game::gunshot0 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+50, _top+20, "n2", 16);
}
void Game::gunshot1 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+25+(rand()%54), _top+20, "n2", 16);
}
void Game::gunshot2 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+(rand()%104), _top+20, "n2", 16);
}
void Game::laser0 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+50, _top+20, "n2", 48);
}
void Game::laser1 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+50, _top+20, "n2", 48, 25);
}
void Game::laser2 (int _ts, float _left, float _top)
{
    bullet01(_ts, _left+50, _top+20, "n2", 48, 60);
}

void Game::bullet02(int _ts, float _left, float _top, const std::string & _fashion)
{
    char name[128]; snprintf(name, 128, "bullet%05d", bulletId);
    bulletId = (bulletId+1)%100000;
    splashouille::Object * object = engine->getLibrary()->copyObject("_bullet", name);
    object->getStyle()->setTop(_top - object->getStyle()->getHeight()/2);
    object->getStyle()->setLeft(_left - object->getStyle()->getWidth()/2);
    object->changeFashion(_fashion);
    object->setState(1);
    a_bullets->getCrowd()->insertObject(_ts, object);
}

void Game::bulletSimple(int _ts, float _left, float _top)
{
    bullet02(_ts, _left, _top, "s");
}
void Game::bulletDuo(int _ts, float _left, float _top)
{
    bullet02(_ts, _left-16, _top, "s");
    bullet02(_ts, _left+16, _top, "s");
}
void Game::bulletDuoSW(int _ts, float _left, float _top)
{
    bullet02(_ts, _left-16, _top, "sw");
    bullet02(_ts, _left+16, _top, "sw");
}
void Game::bulletDuoSE(int _ts, float _left, float _top)
{
    bullet02(_ts, _left-16, _top, "se");
    bullet02(_ts, _left+16, _top, "se");
}
void Game::bulletCross4(int _ts, float _left, float _top)
{
    bullet02(_ts, _left, _top, "s");
    bullet02(_ts, _left, _top, "e");
    bullet02(_ts, _left, _top, "w");
    bullet02(_ts, _left, _top, "n");
}
void Game::bulletCross8(int _ts, float _left, float _top)
{
    bulletCross4(_ts, _left, _top);
    bullet02(_ts, _left, _top, "se");
    bullet02(_ts, _left, _top, "sw");
    bullet02(_ts, _left, _top, "ne");
    bullet02(_ts, _left, _top, "nw");
}
void Game::bulletCross16(int _ts, float _left, float _top)
{
    bulletCross8(_ts, _left, _top);
    bullet02(_ts, _left, _top, "sse");
    bullet02(_ts, _left, _top, "see");
    bullet02(_ts, _left, _top, "ssw");
    bullet02(_ts, _left, _top, "sww");
    bullet02(_ts, _left, _top, "nne");
    bullet02(_ts, _left, _top, "nee");
    bullet02(_ts, _left, _top, "nnw");
    bullet02(_ts, _left, _top, "nww");
}
void Game::bulletCross16Bis(int _ts, float _left, float _top)
{
    bullet02(_ts, _left, _top, "d01");
    bullet02(_ts, _left, _top, "d03");
    bullet02(_ts, _left, _top, "d05");
    bullet02(_ts, _left, _top, "d07");
    bullet02(_ts, _left, _top, "d09");
    bullet02(_ts, _left, _top, "d11");
    bullet02(_ts, _left, _top, "d13");
    bullet02(_ts, _left, _top, "d15");
    bullet02(_ts, _left, _top, "d17");
    bullet02(_ts, _left, _top, "d19");
    bullet02(_ts, _left, _top, "d21");
    bullet02(_ts, _left, _top, "d23");
    bullet02(_ts, _left, _top, "d25");
    bullet02(_ts, _left, _top, "d27");
    bullet02(_ts, _left, _top, "d29");
    bullet02(_ts, _left, _top, "d31");
}
void Game::bulletCross32(int _ts, float _left, float _top)
{
    bulletCross16(_ts, _left, _top);
    bulletCross16Bis(_ts, _left, _top);
}

void Game::bulletDir00(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "n"); }
void Game::bulletDir01(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d01"); }
void Game::bulletDir02(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "nne"); }
void Game::bulletDir03(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d03"); }
void Game::bulletDir04(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "ne"); }
void Game::bulletDir05(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d05"); }
void Game::bulletDir06(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "nee"); }
void Game::bulletDir07(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d07"); }
void Game::bulletDir08(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "e"); }
void Game::bulletDir09(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d09"); }
void Game::bulletDir10(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "see"); }
void Game::bulletDir11(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d11"); }
void Game::bulletDir12(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "se"); }
void Game::bulletDir13(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d13"); }
void Game::bulletDir14(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "sse"); }
void Game::bulletDir15(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d15"); }
void Game::bulletDir16(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "s"); }
void Game::bulletDir17(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d17"); }
void Game::bulletDir18(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "ssw"); }
void Game::bulletDir19(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d19"); }
void Game::bulletDir20(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "sw"); }
void Game::bulletDir21(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d21"); }
void Game::bulletDir22(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "sww"); }
void Game::bulletDir23(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d23"); }
void Game::bulletDir24(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "w"); }
void Game::bulletDir25(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d25"); }
void Game::bulletDir26(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "nww"); }
void Game::bulletDir27(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d27"); }
void Game::bulletDir28(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "nw"); }
void Game::bulletDir29(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d29"); }
void Game::bulletDir30(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "nnw"); }
void Game::bulletDir31(int _ts, float _left, float _top) { bullet02(_ts, _left, _top, "d31"); }


void Game::bulletMidCross8(int _ts, float _left, float _top)
{
    bullet02(_ts, _left, _top, "s");
    bullet02(_ts, _left, _top, "e");
    bullet02(_ts, _left, _top, "w");
    bullet02(_ts, _left, _top, "se");
    bullet02(_ts, _left, _top, "sw");
}
void Game::bulletMidCross16(int _ts, float _left, float _top)
{
    bulletMidCross8(_ts, _left, _top);
    bullet02(_ts, _left, _top, "sse");
    bullet02(_ts, _left, _top, "see");
    bullet02(_ts, _left, _top, "ssw");
    bullet02(_ts, _left, _top, "sww");
}








