#include "BasicCannonBall.h"
#include "Cannon.h"
#include "GameEngine.h"
#include "ParticlePool.h"
#include "SoundManager.h"
#include "Square.h"
#include "Utility.h"
#include "World.h"
#include "PlayGameState.h"

#include <Ogre.h>

BasicCannonBall::BasicCannonBall(Cannon* c)
{
	Ogre::Entity* entity;
	cannon = c;
	root = cannon->getTube()->createChildSceneNode(generateId(), Ogre::Vector3(0, 0, 0));
	explosionHolder = GameEngine::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode(generateId());
	entity = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "CannonBall.mesh");
	root->attachObject(entity);
	root->setScale(0.5f, 0.5f, 0.5f);
	root->setVisible(false);

	ready = true;
	velocity = Ogre::Vector3(0.0, 0.0, 0.0);
}

void BasicCannonBall::launch()
{
	root->setVisible(true);
	Ogre::Vector3 initialPos = root->getWorldPosition();
	ready = false;
	cannon->getTube()->removeChild(root->getName());
	GameEngine::getSingleton().getSceneManager()->getRootSceneNode()->addChild(root);
	root->setPosition(initialPos);
	root->setScale(0.2f, 0.2f, 0.2f);
	Ogre::Real v = cannon->getInitialVelocity();
	Ogre::Radian phi = cannon->getStand()->getOrientation().getYaw();
	Ogre::Radian theta = cannon->getTube()->getOrientation().getPitch();
	velocity.x = v * Ogre::Math::Cos(theta) * Ogre::Math::Sin(phi);
	velocity.z = v * Ogre::Math::Cos(theta) * Ogre::Math::Cos(phi);
	velocity.y = -1 * v * Ogre::Math::Sin(theta);
}

void BasicCannonBall::hit()
{
	Ogre::Vector3 hitPos = root->getWorldPosition();
	Square* hitSquare = cannon->getSquare()->getWorld()->getSquareO(hitPos);
	Ogre::Real oceanLevel = static_cast<PlayGameState*>(GameEngine::getSingletonPtr()->getState("play"))->getOceanLevel();

	explosionHolder->setPosition(hitPos);
	
	if(hitPos.y < oceanLevel - 5.0)
	{
		hitPos.y = oceanLevel;
		cannon->getSquare()->getWorld()->getParticlePool("WaterSplash")->attachParticleTo(explosionHolder);
		GameEngine::getSingleton().getSoundManager()->play("Splash");
	}
	else
	{
		cannon->getSquare()->getWorld()->getParticlePool("CannonBallHit")->attachParticleTo(explosionHolder);
		GameEngine::getSingleton().getSoundManager()->play("GroundHit");

		// TODO add support for textures whose size is other than 512x512 and for ground whose size is other than 3000x3000
		Ogre::Vector2 texturePos((512.0/3000.0) * hitPos.x, (512.0/3000.0) * hitPos.z);
		if(texturePos.x <= 512.0 && texturePos.x >= 0.0 && texturePos.y <= 512.0 && texturePos.y >= 0)
		{
			Ogre::Image img;
			img.load("splat.png", "Default");
			img.resize(10, 10);
			
			switch (rand() % 3)
			{
			case 1:
				img.flipAroundX();
				break;
			case 2:
				img.flipAroundY();
				break;
			case 3:
				img.flipAroundX();
				img.flipAroundY();
				break;
			}

			// Blit the loaded image into the terrain texture. We are using hardware pixel buffers directly
			// to obtain flexibility.
			// This works for all kinds of images that has only three channels, that is it doesn't have alpha channel
			// TODO add support for images with alpha channel.
			// TODO add support for texture whose size is other than 512x512x4
			Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName("SplattedTerrain");
			Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
			pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
			const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
			Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
			Ogre::uint8* pSource = static_cast<Ogre::uint8*>(img.getPixelBox().data);

			// Loop through every pixel in terrain texture
			for(size_t i = 0; i < 512; i++)
			{
				for(size_t j = 0; j < 512; j++)
				{
					if(j >= texturePos.x - 5 && j <= texturePos.x + 5 && i >= texturePos.y - 5 && i <= texturePos.y + 5)
					{
						for(size_t k = 0; k < 3; ++k)
						{
							// Add pixels from source image only if they are darker than the ones that already are in the terrain texture.
							if(*pDest < *pSource)
							{
								pDest++;
								pSource++;
							}
							else
							{
								*pDest++ = *pSource++;
							}
						}
						// Source image doesnt't have alpha channel, destination texture has.
						pDest++;
					}
					else
					{
						pDest++;
						pDest++;
						pDest++;
						pDest++;
					}
				}
			}
			pixelBuffer->unlock();
		}	
	}

	if(hitSquare != NULL)
	{
		if(hitSquare->getStructure() != NULL)
		{
			hitSquare->getStructure()->destroy();
		}
	}
	
	GameEngine::getSingleton().getSceneManager()->getRootSceneNode()->removeChild(root->getName());
	cannon->getTube()->addChild(root);
	root->setPosition(0, 0, 0);
	velocity = Ogre::Vector3(0.0, 0.0, 0.0);
	ready = true;
	root->setVisible(false);
}

bool BasicCannonBall::isReady()
{
	return ready;
}

Ogre::Vector3 BasicCannonBall::getVelocity()
{
	return velocity;
}

void BasicCannonBall::setVelocity(Ogre::Vector3 v)
{
	velocity = v;
}

