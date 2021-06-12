#ifndef __CREDITS_SCENE_H__
#define __CREDITS_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "MusicFade.h"

using namespace cocos2d;

class Credits : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Credits);
};

#endif // __CREDITS_SCENE_H__