#ifndef __MUSICFADE_H__
#define __MUSICFADE_H__

#include "cocos2d.h"

class MusicFade : public cocos2d::ActionInterval
{
public:
    MusicFade();

    static MusicFade* create(int target, float d, float volume, bool stopOnComplete = false );
    bool initWithDuration(int target, float d, float volume, bool stopOnComplete );

    virtual void startWithTarget(cocos2d::Node *pTarget);
    virtual void update(float time);
    virtual void stop(void);

protected:
	int m_audioID;
    float m_targetVal;
    float m_initialVal;
    bool m_stopOnComplete;
};

#endif // __MUSICFADE_H__