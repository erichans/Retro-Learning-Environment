/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details.
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 */
#include "../RomUtils.hpp"
#include <iomanip>

#include "../RomUtils.hpp"
#include "RleSystem.hxx"
#include "FZero.hpp"

using namespace rle;


FZeroSettings::FZeroSettings() {
    reset();

    minimalActions = {	JOYPAD_NOOP,
						JOYPAD_LEFT,
						JOYPAD_RIGHT,
						JOYPAD_B,	// speed
						JOYPAD_B | JOYPAD_RIGHT,
						JOYPAD_B | JOYPAD_LEFT,

						JOYPAD_Y,	// break
						JOYPAD_Y | JOYPAD_RIGHT,
						JOYPAD_Y | JOYPAD_LEFT,
						JOYPAD_Y | JOYPAD_R,
						JOYPAD_Y | JOYPAD_L,

						JOYPAD_A | JOYPAD_B, // Turbo
						JOYPAD_A | JOYPAD_RIGHT | JOYPAD_B,
						JOYPAD_A | JOYPAD_LEFT | JOYPAD_B,
						JOYPAD_A | JOYPAD_R | JOYPAD_B,
						JOYPAD_A | JOYPAD_L | JOYPAD_B,

						JOYPAD_R, // lean right
						JOYPAD_R | JOYPAD_B,

						JOYPAD_L, // lean left
						JOYPAD_L | JOYPAD_B,
    };
}


/* create a new instance of the rom */
RomSettings* FZeroSettings::clone() const {
    RomSettings* rval = new FZeroSettings();
    *rval = *this;
    return rval;
}


/* process the latest information from ALE */
void FZeroSettings::step(const RleSystem& system) {
//	uint32_t time = 60 * readRam(&system, 0xAC) + readRam(&system, 0xAE);

	// update the reward
	int isForward = 1; // indicating not driving the opposite direction
	if(5==readRam(&system, 0xB01)){ // the address hold 5 if driving the opposite direction and 4 if driving the correct direction
		isForward=-1;
	}
	int32_t playerScore = 1000*readRam(&system, 0xA23) + 100*readRam(&system, 0xA24) +10*readRam(&system, 0xA25); //give extra points according sto speed to maintain highspeed
	int32_t speedScore = 100*readRam(&system, 0xA58) + 10*readRam(&system, 0xA59) +readRam(&system, 0xA5A); //give extra points according sto speed to maintain highspeed
	int32_t score = playerScore + isForward * speedScore; //adding negative factor if driving the wrong direction
//	uint32_t lives = readRam(&system, 0x59);
//	uint32_t loseInd1 = readRam(&system , 0x41F);
	uint32_t loseInd1 = readRam(&system , 0xC1);
	uint32_t loseInd2 = readRam(&system , 0x50); //lose indication

	uint32_t loseInd = (loseInd2 ==0) && (loseInd1>0); //lose if time has start (larger than 0) and lose indicator is on
	m_reward = score - m_score;
    m_score = score;

    if (loseInd ==1){
    	m_terminal = true;
    }

}

/* reset the state of the game */
void FZeroSettings::reset() {

    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
}



/* saves the state of the rom settings */
void FZeroSettings::saveState( Serializer & ser ) {
  ser.putInt(m_reward);
  ser.putInt(m_score);
  ser.putBool(m_terminal);
}

// loads the state of the rom settings
void FZeroSettings::loadState( Deserializer & des ) {
  m_reward = des.getInt();
  m_score = des.getInt();
  m_terminal = des.getBool();
}


ActionVect FZeroSettings::getStartingActions(){
	int num_of_nops(100);
	ActionVect startingActions;

	// wait for intro to end
	startingActions.insert(startingActions.end(), num_of_nops, JOYPAD_NOOP);
	// press start to begin
	startingActions.push_back(JOYPAD_START);
	// select arcade
	startingActions.insert(startingActions.end(), num_of_nops, JOYPAD_NOOP);
	startingActions.push_back(JOYPAD_START);
	// select 1 vs cpu
	startingActions.insert(startingActions.end(), num_of_nops, JOYPAD_NOOP);
	startingActions.push_back(JOYPAD_START);
	// select team
	startingActions.insert(startingActions.end(), num_of_nops, JOYPAD_NOOP);
	startingActions.push_back(JOYPAD_START);
	// select team
	startingActions.insert(startingActions.end(), num_of_nops, JOYPAD_NOOP);
	startingActions.push_back(JOYPAD_START);
	// wait for race countdown
	startingActions.insert(startingActions.end(), 5.5*num_of_nops, JOYPAD_NOOP);

	return startingActions;
}

