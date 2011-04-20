/*
 *  shape.h
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
class Shape{

public:
	
	static Shape* board[10];
	struct CIRCLE{
		float x;
		float y;
		float alpha;
	};
	struct CIRCLE* bubbles[10];
	
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
	void checkCollision(int index);
	void grabShape(int hand);
	void releaseShapeFrom(int hand);
	bool isGrabbed();
	bool isGrabbedBy(int hand);
	bool hoveredOver(float x, float y);
	void slow();
	void collision_Bounce(int i);
	void collision_AntiMagnet(float x, float y);
	bool notMoving();
	void velocityCeiling();
	void createExplosion();
	
	bool explosion();
	void setVelocity(float vel_x, float vel_y);
	float getVelocity_y();
	float getVelocity_x();
	void setColor(float R, float G, float B, float A);
	void setLocation(float x, float y);

private:
	bool outsideBounds(float left, float right, float top, float bottom);
	bool outsideBounds_x(float left, float right);
	bool outsideBounds_y( float top, float bottom);
	bool insideBounds(float left, float right, float top, float bottom);
	bool insideBounds_x(float left, float right);
	bool insideBounds_y( float top, float bottom);
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

