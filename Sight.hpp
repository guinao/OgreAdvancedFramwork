#ifndef __Sight_hpp_
#define __Sight_hpp_

#include "OgreManualObject.h"

class Sight : public Ogre::ManualObject
{
public :
	Sight(const Ogre::String& name);
	~Sight(void);

	// ��һ��Բ����Ϊ�����õ��������������Ҫ���볤�Ϳ�
	void setCenter(float x, float y, float w, float h);

};

#endif
