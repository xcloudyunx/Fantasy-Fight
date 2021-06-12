#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"
#include "HomeScene.h"
#include "AudioEngine.h"

using namespace cocos2d;

class Loading : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Loading);
};

#endif // __LOADING_SCENE_H__
