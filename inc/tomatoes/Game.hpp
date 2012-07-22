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

#ifndef TOMATOES_GAME_HPP_
#define TOMATOES_GAME_HPP_

#include <splashouille/Engine.hpp>
#include <splashouille/Crowd.hpp>

#include <list>

namespace splashouille
{
class Animation;
class Object;
class Engine;
}

namespace tomatoes
{
class Tomatoes;


/**
 * Class menu handles all the menu interactions
 */
class Game : public splashouille::Engine::Listener, public splashouille::Crowd::Listener
{
private:

    typedef enum
    {
        level0              = 0,                // NOT INTERACTIVE LEVEL
        level1              = 10,               // INSERT THE AIRCRAFTS
        level2              = 20,               // PLAY
        last                = 200
    } state;

    typedef enum
    {
        allies              = 0,
        bullets             = 1,
        ennemies            = 2,
        effects             = 3
    } crowdused;

    typedef enum
    {
        none                = 0,
        simple              = 1,
        duo                 = 2,
        duosw               = 3,
        duose               = 4,
        cross4              = 5,
        cross8              = 6,
        cross16             = 7,
        cross16bis          = 8,
        cross32             = 9,
        dir00               = 10,
        dir01               = 11,
        dir02               = 12,
        dir03               = 13,
        dir04               = 14,
        dir05               = 15,
        dir06               = 16,
        dir07               = 17,
        dir08               = 18,
        dir09               = 19,
        dir10               = 20,
        dir11               = 21,
        dir12               = 22,
        dir13               = 23,
        dir14               = 24,
        dir15               = 25,
        dir16               = 26,
        dir17               = 27,
        dir18               = 28,
        dir19               = 29,
        dir20               = 30,
        dir21               = 31,
        dir22               = 32,
        dir23               = 33,
        dir24               = 34,
        dir25               = 35,
        dir26               = 36,
        dir27               = 37,
        dir28               = 38,
        dir29               = 39,
        dir30               = 40,
        dir31               = 41,
        midcross8           = 98,
        midcross16          = 99,
        lastbullet
    } bullet;

    typedef void (Game::*p_onFrame)(int _frame, int _timeStampInMilliSeconds);
    typedef void (Game::*p_fire)(int _ts, float _left, float _top);
    typedef void (Game::*p_bullet)(int _ts, float _left, float _top);

public:
    const static int        nbModels = 3;

    /**
     * Class player
     * TODO: Move from Game to somewhere (Main?)
     */
    class Player
    {
    public:

        typedef enum
        {
            gunshot             = 0,
            trident             = 1,
            laser               = 2,
            lastfire            = 3
        } fire;

        class Model
        {
        public:
            float           acceleration;
            float           deceleration;
            float           maxSpeed;
            float           weight;
            fire            fireType;
            int             life;
            int             offset[2];
            std::string     name;
            float           radius2;

            Model(float _a, float _d, float _m, float _w, fire _f, int _l, int _ox, int _oy, const std::string& _n):
                acceleration(_a), deceleration(_d), maxSpeed(_m), weight(_w), fireType(_f), life(_l), name(_n), radius2(16)
            {
                offset[0] = _ox; offset[1] = _oy;
            }
        };

        static const int    nbMaxLevels = 3;
        static const int    delayFire[lastfire][nbMaxLevels];
        static const Model  models[nbModels];
    private:
        Game *                  parent;                             // The game parent
        splashouille::Object *  object;                             // The splashouille sprite
        int                     model;                              // The player model
        int                     timestamp;                          // The last compute timestamp;
        int                     life;                               // The current life
        int                     level;                              // The current level
        int                     lastBulletTs;                       // The last bullet timestamp
        int                     lastHit;                            // The last hit timestamp
        float                   speed[2];                           // The current speed
    public:
        Player(): parent(0), object(0), timestamp(0), level(0), lastBulletTs(0), lastHit(0) { speed[0] = speed[1] = 0; }
        void init(Game * _parent, int _ts, int _playerId, int _model, bool _multi);
        void update(int _ts, bool _up, bool _right, bool _down, bool _left, bool _fire);
        void getPosition(float &_x, float &_y);
        void addSpeed(float _x, float _y);
        bool hit(int _ts, int _value);
        const Model& getModel() { return models[model]; }
        bool isAlive() { return (life>0); }
        void levelUp() { if (level<2) { level++; } }
    };

private:
    Tomatoes *                  tomatoes;                           // The tomatoes parent
    splashouille::Engine *      engine;                             // The menu splashouille engine
    splashouille::Animation *   a_shoots;                           // Players bullets animation
    splashouille::Animation *   a_ennemies;                         // The tomatoes ennemies
    splashouille::Animation *   a_bullets;                          // The tomatoes bullets
    splashouille::Animation *   a_effects;                          // The effects animation
    p_onFrame                   onFrameMethods[last];               // onFrame callbacks regarding the state
    p_fire                      fireMethods[Player::lastfire*100];  // fire methods
    p_bullet                    bulletMethods[lastbullet];          // bullet methods
    Player                      players[2];                         // The players
    unsigned int                bulletId;                           // An bullets counter
    int                         currentTs;                          // The current timestamp

    std::list<splashouille::Object*>    alliesToDelete;             // The players bullets to delete
    std::list<splashouille::Object*>    ennemiesToDelete;           // The ennemies tomatoes to delete
    std::list<splashouille::Object*>    bulletsToDelete;            // The ennemies bullets to delete
    std::list<splashouille::Object*>    effectsToDelete;            // The effects to delete
    std::list<splashouille::Object*>    alliesArray[10][10];        // The players bullets array

    /** Menu event handling */
    void                        onLevel1 (int _frame, int _timeStampInMilliSeconds);
    void                        onLevel2 (int _frame, int _timeStampInMilliSeconds);

    /** Fire methods */
    int                         getFireId(Player::fire _fireType, int _level) { return _fireType*100+_level; }
    void                        bullet01 (int _ts, float _left, float _top, const std::string & _fashion, int _positionY, int _power = 10);
    void                        trident0 (int _ts, float _left, float _top);
    void                        trident1 (int _ts, float _left, float _top);
    void                        trident2 (int _ts, float _left, float _top);
    void                        gunshot0 (int _ts, float _left, float _top);
    void                        gunshot1 (int _ts, float _left, float _top);
    void                        gunshot2 (int _ts, float _left, float _top);
    void                        laser0 (int _ts, float _left, float _top);
    void                        laser1 (int _ts, float _left, float _top);
    void                        laser2 (int _ts, float _left, float _top);

    /** Ennemies bullet methods */
    void                        bullet02(int _ts, float _left, float _top, const std::string & _fashion);
    void                        bulletSimple(int _ts, float _left, float _top);
    void                        bulletDuo(int _ts, float _left, float _top);
    void                        bulletDuoSW(int _ts, float _left, float _top);
    void                        bulletDuoSE(int _ts, float _left, float _top);
    void                        bulletCross4(int _ts, float _left, float _top);
    void                        bulletCross8(int _ts, float _left, float _top);
    void                        bulletCross16(int _ts, float _left, float _top);
    void                        bulletCross16Bis(int _ts, float _left, float _top);
    void                        bulletCross32(int _ts, float _left, float _top);
    void                        bulletMidCross8(int _ts, float _left, float _top);
    void                        bulletMidCross16(int _ts, float _left, float _top);
    void                        bulletDir00(int _ts, float _left, float _top);
    void                        bulletDir01(int _ts, float _left, float _top);
    void                        bulletDir02(int _ts, float _left, float _top);
    void                        bulletDir03(int _ts, float _left, float _top);
    void                        bulletDir04(int _ts, float _left, float _top);
    void                        bulletDir05(int _ts, float _left, float _top);
    void                        bulletDir06(int _ts, float _left, float _top);
    void                        bulletDir07(int _ts, float _left, float _top);
    void                        bulletDir08(int _ts, float _left, float _top);
    void                        bulletDir09(int _ts, float _left, float _top);
    void                        bulletDir10(int _ts, float _left, float _top);
    void                        bulletDir11(int _ts, float _left, float _top);
    void                        bulletDir12(int _ts, float _left, float _top);
    void                        bulletDir13(int _ts, float _left, float _top);
    void                        bulletDir14(int _ts, float _left, float _top);
    void                        bulletDir15(int _ts, float _left, float _top);
    void                        bulletDir16(int _ts, float _left, float _top);
    void                        bulletDir17(int _ts, float _left, float _top);
    void                        bulletDir18(int _ts, float _left, float _top);
    void                        bulletDir19(int _ts, float _left, float _top);
    void                        bulletDir20(int _ts, float _left, float _top);
    void                        bulletDir21(int _ts, float _left, float _top);
    void                        bulletDir22(int _ts, float _left, float _top);
    void                        bulletDir23(int _ts, float _left, float _top);
    void                        bulletDir24(int _ts, float _left, float _top);
    void                        bulletDir25(int _ts, float _left, float _top);
    void                        bulletDir26(int _ts, float _left, float _top);
    void                        bulletDir27(int _ts, float _left, float _top);
    void                        bulletDir28(int _ts, float _left, float _top);
    void                        bulletDir29(int _ts, float _left, float _top);
    void                        bulletDir30(int _ts, float _left, float _top);
    void                        bulletDir31(int _ts, float _left, float _top);


public:
    Game(Tomatoes * _tomatoes):tomatoes(_tomatoes), engine(0), bulletId(0), currentTs(0) {}

    /** Init the game */
    void init(int _level);

    /** Is ready */
    bool isReady() { return engine->getProgress()==100; }

    /** Run the class menu */
    void run(SDL_Surface * _screen);

    /** Fire bullets */
    void fire(int _ts, float _left, float _top, Player::fire _fireType, int _level);

    /** Crowd callback */
    bool onObject(splashouille::Object * _object, int _user);

    /**
     * Callback called each second
     * @param _frame is the frame number from the beginning of the animation
     * @param _frameSec is the number of frames played during the last second
     * @param _second is the current second
     */
    void onSecond(int _frame, int _frameSec, int _second);

    /**
     * Callback called on each frame
     * @param _frame is the frame number from the beginning of the animation
     * @param _timeStampInMilliSeconds is the current timestamp
     */
    void onFrame(int _frame, int _timeStampInMilliSeconds);

    /**
     * The onEvent Callback
     * @param _event is the SDL_Event
     */
    bool onEvent(SDL_Event & _event, int _timeStampInMilliSeconds);

    /**
     * Callback on the quit event
     * @return true if the event is consumed
     */
    bool onStop() { return true; }
};

}


#endif
