#include "Sight.hpp"

Sight::Sight(const Ogre::String& name)
	: Ogre::ManualObject(name)
{
	setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY); // when using this, ensure Depth Check is Off in the material
	setUseIdentityProjection(true);
	setUseIdentityView(true);
	setQueryFlags(0);
}

Sight::~Sight()
{
}

void Sight::setCenter(float x, float y, float w, float h)
{
	clear();
	begin("", Ogre::RenderOperation::OT_LINE_STRIP);
	end();
	setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
}