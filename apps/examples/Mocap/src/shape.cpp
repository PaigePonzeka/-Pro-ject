/*
 *  shape.cpp
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "shape.h"


Shape::Shape(float init_location_x,float init_location_y, float init_height, float init_width, 
			 float init_R, float init_G, float init_B, float init_A)
{
	location_x = init_location_x;
	location_y = init_location_y;
	height = init_height;
	width = init_width;
	color.red = init_R;
	color.green = init_G;
	color.blue = init_B;
	color.alpha = init_A;
	
}

void Shape::move()
{
	location_x = location_x + velocity_x;
	location_y = location_x + velocity_x;
}

bool Shape::willCollide()
{
}

CvPoint Shape::getLocation()
{
	return cvPoint(location_x, location_y);
}

CvPoint2D32f Shape::getLocation2D32f()
{
	return cvPointFrom32f( cvPoint2D32f( location_x, location_y ) );

}

float Shape::getLocation_x()
{
	return location_x;
}

float Shape::getLocation_y()
{
	return location_y;
}

void Shape::setVelocity(float vel_x, float vel_y)
{
	velocity_x = vel_x;
	velocity_y = vel_y;
}

void Shape::setColor(float R, float G, float B, float A)
{
	color.red = R;
	color.green = G;
	color.blue = B;
	color.alpha = A;
}