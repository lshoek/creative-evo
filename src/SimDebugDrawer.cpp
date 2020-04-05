#pragma once
#include "SimDebugDrawer.h"
#include "SimUtils.h"

void SimDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	ofDrawLine(SimUtils::bulletToGlm(from), SimUtils::bulletToGlm(to));
};

void SimDebugDrawer::setDefaultColors(const DefaultColors&) {};
void SimDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) 
{
	ofSetHexColor(0xff0088);
	ofDrawLine(SimUtils::bulletToGlm(PointOnB), SimUtils::bulletToGlm(PointOnB + normalOnB*distance));
	ofSetHexColor(0xffffff);
};

void SimDebugDrawer::reportErrorWarning(const char* warningString) {};
void SimDebugDrawer::draw3dText(const btVector3& location, const char* textString) {};

void SimDebugDrawer::setDebugMode(int debugMode)
{
	_dbgDrawMode = debugMode;
};

int SimDebugDrawer::getDebugMode() const
{ 
	return _dbgDrawMode; 
};