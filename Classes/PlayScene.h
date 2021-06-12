#ifndef __PLAY_SCENE_H__
#define __PLAY_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "Player.h"
#include <map>
#include <stdlib.h>

using namespace cocos2d;

class Play : public Scene {
public:
	static Scene* createScene();

	virtual bool init();
	
	void customFade(float);

	CREATE_FUNC(Play);

private:
	Player* _playerOne;
	Player* _playerTwo;
	
	bool _paused = false;
	Vector<Node*> _pausedActions;
	std::vector<int> _marker;
	bool _cooldown = false;
	void move(int);

	void update(float);
	void cFade(float);
};

#endif // __PLAY_SCENE_H__
