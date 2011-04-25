/*
 *  shape.h
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "ofMath.h"

class Shape{

public:
	
	//the board of 10 shapes
	static Shape* board[10];
	struct SHADOW{
		float x;
		float y;
		float alpha;
		float size;
	};
	
	//array of shards of the shape
	struct SHADOW* bubbles[10];
	struct SHADOW* trail[10];
	
	Shape();
	Shape(float init_location_x,float init_location_y,float init_height, float init_width,  
		  float init_R, float init_G, float init_B, float init_A);
	
	
	float getLocation_x();
	float getLocation_y();
	float getWidth();
	float getHeight();
	float getRed();
	float getGreen();
	float getBlue();
	float getAlpha();
	
	void move();
	void slow();
	bool hoveredOver(float x, float y);

	//collisions
	void checkCollision(int index);
	void collision_Bounce(int i);
	void collision_AntiMagnet(float x, float y);
	bool locationError(int i, bool allow_one);
	void checkDamage();

	//explosions
	void createExplosion();
	bool isExploding();
	void doneExploding();
	bool popBubbles();

	//trail
	void updateTrail();
	int index();
	
	//getters and setters
	void setVelocity(float vel_x, float vel_y);
	void incVelocity(float vel_x, float vel_y);
	float getVelocity_y();
	float getVelocity_x();
	void setColor(float R, float G, float B, float A);
	void setLocation(float x, float y);
	
	//--- possibly deletable methods ---
	void grabShape(int hand);
	void releaseShapeFrom(int hand);
	bool isGrabbed();
	bool isGrabbedBy(int hand);
	//----------------------------------
	
	

private:
	bool outsideBounds(float left, float right, float top, float bottom);
	bool outsideBounds_x(float left, float right);
	bool outsideBounds_y( float top, float bottom);
	bool insideBounds(float left, float right, float top, float bottom);
	bool insideBounds_x(float left, float right);
	bool insideBounds_y( float top, float bottom);
	int trail_index;
	int hit_count;
	float height;
	float width;
	float location_x;
	float location_y;
	float velocity_x;
	float velocity_y;
	bool grabbed;
	bool exploding;
	int grabbed_by;
	struct COLOR{
		float red;
		float green;
		float blue;
		float alpha;
	};
	struct COLOR shape_color;
	
};

