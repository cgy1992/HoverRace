
// TestElement.cpp
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which
// you have taken this file. If you can not find the license you can not use
// this file.
//
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions
// and limitations under the License.
//

#include "../Model/ObstacleCollisionReport.h"
#include "../Model/Track.h"
#include "../ObjFacTools/ResourceLib.h"

#include "TestElement.h"

using HoverRace::ObjFacTools::ResourceLib;

namespace HoverRace {
namespace ObjFac1 {

namespace {

MR_UInt16 ActorResToId(int actorRes) {
	switch (actorRes) {
		case MR_DEMO_FIGHTER: return 10;
		case MR_ELECTRO_CAR: return 13;
	}
	throw UnimplementedExn("ActorResToId: actor resource: " +
		boost::lexical_cast<std::string>(actorRes));
}

}  // namespace

TestElement::TestElement(ResourceLib &resourceLib, int pActorRes) :
	SUPER({ 1, ActorResToId(pActorRes) }),
	mElapsedFrameTime(0),
	mXSpeed(0), mYSpeed(0)
{
	mActor = resourceLib.GetActor(pActorRes);

	mCollisionShape.mRay = 250;
	mContactShape.mRay = 300;

	mContactEffect.mWeight = 90;
	mContactEffectList.push_back(&mContactEffect);
}

int TestElement::Simulate(MR_SimulationTime pDuration,
	Model::Track &track, int pRoom)
{
	auto level = track.GetLevel();

	mElapsedFrameTime += pDuration;

	int lFrameIncrement = mElapsedFrameTime / 75;

	if(lFrameIncrement > 0) {
		mElapsedFrameTime %= 75;

		mCurrentFrame += lFrameIncrement;

		if(mCurrentFrame >= mActor->GetFrameCount(mCurrentSequence)) {
			mCurrentFrame = 0;
			mCurrentSequence++;

			if(mCurrentSequence >= mActor->GetSequenceCount()) {
				mCurrentSequence = 0;
			}
		}
	}
	// Now do the displacement

	// Compute translation effect
	MR_3DCoordinate lTranslation((mXSpeed * (int) pDuration) / 256, (mYSpeed * (int) pDuration) / 256, (-2 * 256 * (int) pDuration) / 256);

	// Verify if the move is valid
	Cylinder lShape;
	MR_3DCoordinate lNewPos;
	MR_Angle lNewOrientation;
	Model::ObstacleCollisionReport lReport;

	lNewPos = mPosition;
	lNewOrientation = mOrientation;

	lNewPos.mX += lTranslation.mX;
	lNewPos.mY += lTranslation.mY;
	lNewPos.mZ += lTranslation.mZ;

	lShape.mRay = mCollisionShape.mRay;
	lShape.mPosition = lNewPos;

	lReport.GetContactWithObstacles(level, &lShape, pRoom, this);

	if(lReport.IsInMaze()) {
		if(!lReport.HaveContact()) {
			mPosition = lNewPos;
			mOrientation = lNewOrientation;
			pRoom = lReport.Room();
		}
		else {
			// Determine if we can go on the object
			if((lReport.SpaceToCeiling() > 0) && (lReport.StepHeight() < (100))) {
				lNewPos.mZ += lReport.StepHeight() + 1;

				lShape.mPosition = lNewPos;

				lReport.GetContactWithObstacles(level, &lShape, pRoom, this);

				if(lReport.IsInMaze()) {
					if(!lReport.HaveContact()) {
						mPosition = lNewPos;
						mOrientation = lNewOrientation;
						pRoom = lReport.Room();
					}
				}
			}
		}
	}
	return pRoom;
}

const Model::ShapeInterface *TestElement::GetObstacleShape()
{
	mCollisionShape.mPosition = mPosition;

	return &mCollisionShape;
}

void TestElement::ApplyEffect(const Model::ContactEffect *pEffect,
	MR_SimulationTime, MR_SimulationTime,
	BOOL pValidDirection,
	MR_Angle pHorizontalDirection,
	MR_Int32, MR_Int32, Model::Track&)
{
	using namespace Model;

	const PhysicalCollision *lPhysCollision =
		dynamic_cast<const PhysicalCollision*>(pEffect);

	if (lPhysCollision && pValidDirection) {
		InertialMoment lMoment;

		lMoment.mWeight = 90;
		lMoment.mXSpeed = mXSpeed;
		lMoment.mYSpeed = mYSpeed;
		lMoment.mZSpeed = 0;

		lMoment.ComputeCollision(lPhysCollision, pHorizontalDirection);

		mXSpeed = lMoment.mXSpeed;
		mYSpeed = lMoment.mYSpeed;

	}

}

const Model::ContactEffectList *TestElement::GetEffectList()
{
	*(int *) &mContactEffect.mXSpeed = mXSpeed;
	*(int *) &mContactEffect.mYSpeed = mYSpeed;
	*(int *) &mContactEffect.mZSpeed = 0;

	return &mContactEffectList;

}

const Model::ShapeInterface *TestElement::GetReceivingContactEffectShape()
{
	mCollisionShape.mPosition = mPosition;
	return &mCollisionShape;
}

const Model::ShapeInterface *TestElement::GetGivingContactEffectShape()
{
	mContactShape.mPosition = mPosition;
	return &mContactShape;
}

}  // namespace ObjFac1
}  // namespace HoverRace
