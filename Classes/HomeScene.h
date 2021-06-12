#ifndef __HOME_SCENE_H__
#define __HOME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ChampScene.h"
#include "SettingsScene.h"
#include "InstructionsScene.h"
#include "CreditsScene.h"
#include "AudioEngine.h"
#include "MusicFade.h"

using namespace cocos2d;

class Home : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Home);
	
private:
	std::vector<int> _marker;
	bool _cooldown = false;
	
	void update(float);
	void move(int);
};

#endif // __HOME_SCENE_H__
