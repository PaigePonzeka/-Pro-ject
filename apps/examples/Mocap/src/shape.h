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
	
	Shape(float init_location_x,float init_location_y,float init_height, float init_width,  
		  float init_R, float init_G, float init_B, float init_A);
	
	//multiple ways to retrieve location data
	CvPoint getLocation();
	CvPoint2D32f getLocation2D32f();	
	float getLocation_x();
	float getLocation_y();
	
	void move();
	bool willCollide();
	
	void setVelocity(float vel_x, float vel_y);
	void setColor(float R, float G, float B, float A);
	

private:
	
	float height;
	float width;
	float location_x;
	float location_y;
	float velocity_x;
	float velocity_y;
	
	struct color{
		float red;
		float green;
		float blue;
		float alpha;
	};
};

