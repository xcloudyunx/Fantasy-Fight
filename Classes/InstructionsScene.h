#ifndef __INSTRUCTIONS_SCENE_H__
#define __INSTRUCTIONS_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "MusicFade.h"

using namespace cocos2d;

class Instructions : public Scene {
public:
	static Scene* createScene();

	virtual bool init();

    CREATE_FUNC(Instructions);
};

#endif // __INSTRUCTIONS_SCENE_H__