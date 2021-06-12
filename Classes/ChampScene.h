#ifndef __CHAMP_SCENE_H__
#define __CHAMP_SCENE_H__

#include "cocos2d.h"
#include "PlayScene.h"
#include "AudioEngine.h"
#include "MusicFade.h"

using namespace cocos2d;

class Champ : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Champ);

private:
	std::vector<int> _playerOne;
	std::vector<int> _playerTwo;
	bool _lockedOne = false;
	bool _lockedTwo = false;
	bool _cooldownOne = false;
	bool _cooldownTwo = false;

	void update(float);
	void move(int, int);
	Vector<SpriteFrame*> getAnimation(std::string, int);
};

#endif // __CHAMP_SCENE_H__
