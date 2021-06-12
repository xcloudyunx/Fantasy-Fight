#ifndef __SETTINGS_SCENE_H__
#define __SETTINGS_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "MusicFade.h"

using namespace cocos2d;

class Settings : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Settings);

private:
	std::vector<int> _marker;
	bool _cooldown = false;
	
	void update(float);
	void move(int);
};

#endif // __SETTINGS_SCENE_H__