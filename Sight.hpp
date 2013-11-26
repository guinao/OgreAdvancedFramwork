#ifndef __Sight_hpp_
#define __Sight_hpp_

#include "OgreManualObject.h"

class Sight : public Ogre::ManualObject
{
public :
	Sight(const Ogre::String& name);
	~Sight(void);

	// 画一个圆，因为这里用的是相对坐标所以要传入长和宽
	void setCenter(float x, float y, float w, float h);

};

#endif
