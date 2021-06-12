#ifndef __OVER_SCENE_H__
#define __OVER_SCENE_H__

#include "cocos2d.h"
#include "MusicFade.h"
#include "AudioEngine.h"

using namespace cocos2d;

class Over : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Over);

private:
	std::vector<int> _marker;
	bool _cooldown = false;
	
	void update(float);
	void move(int);
};

#endif // __OVER_SCENE_H__