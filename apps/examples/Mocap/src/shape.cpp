/*
 *  shape.cpp
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "shape.h"

const float RWALL = 1024.0f; 
const float LWALL = 0.0f;
const float TWALL = 0.0f;
const float BWALL = 768.0f;

Shape::Shape(){}
Shape::Shape(float init_location_x,float init_location_y, float init_height, float init_width, 
			 float init_R, float init_G, float init_B, float init_A)
{
	location_x = init_location_x;
	location_y = init_location_y;
	height = init_height;
	width = init_width;
	shape_color.red = init_R;
	shape_color.green = init_G;
	shape_color.blue = init_B;
	shape_color.alpha = init_A;
	velocity_x = 0;
	velocity_x = 0;
	
}

void Shape::move()
{
	location_x += velocity_x;
	location_y += velocity_y;
	if(outsideBounds_x(LWALL,RWALL)){
		location_x-=velocity_x;
		velocity_x /= -1;
	}
	if(outsideBounds_y(TWALL,BWALL)){
		location_y-=velocity_y;
		velocity_y/= -1;
	}
}
void Shape::slow()
{	
	if(velocity_x!=0){
		if((velocity_x<1)&&(velocity_x>-1)) velocity_x = 0;
		if(velocity_x>0)velocity_x--;
		if(velocity_x<0)velocity_x++;
	}
	if(velocity_y!=0){
		if((velocity_y<1)&&(velocity_y>-1)) velocity_y = 0;
		if(velocity_y>0)velocity_y--;
		if(velocity_y<0)velocity_y++;
	}

}
	


void Shape::checkCollision(int index)
{	float shapeT,shapeB,shapeL,shapeR;
	
	for(int i=0; i<10; i++){
		shapeL = Shape::board[i]->getLocation_x();
		shapeR = shapeL + Shape::board[i]->getWidth();

		shapeT = Shape::board[i]->getLocation_y();
		shapeB = shapeT + Shape::board[i]->getHeight();
		
		//transfer momentum
		if(insideBounds(shapeL, shapeR, shapeT, shapeB)){
			Shape::board[i]->setVelocity(velocity_x, velocity_y);
		}

	}
}
bool Shape::hoveredOver(float x, float y)
{
	float left = location_x;
	float right = location_x + width;
	float top = location_y;
	float bottom = location_y + height;
	
	if((x > left)&&(x < right)&&
	   (y > top)&&(y < bottom)){
		return true;
	}
	else return false;
	
}
		   
bool Shape::outsideBounds(float left, float right, float top, float bottom)
{
	if(((location_x < left)||((location_x + width) > right))&&
	   ((location_y < top)||((location_y + height) > bottom))){
		return true;
	}
	else return false;
}
		   
bool Shape::outsideBounds_x(float left,float right)
{
	if((location_x < left)||((location_x + width) > right)){
		return true;
	}
	else return false;
}
bool Shape::outsideBounds_y(float top, float bottom)
{
	if((location_y < top)||((location_y + height) > bottom)){
		return true;
	}
	else return false;
}

bool Shape::insideBounds(float left, float right, float top, float bottom)
{
	if((insideBounds_x(left, right))&&(insideBounds_y(top, bottom))){
		return true;
	}
	else return false;
}
bool Shape::insideBounds_x(float left, float right)
{
	if((location_x > left)&&((location_x) < right)||
	   ((location_x+width) > left)&&((location_x+width) < right)||
	   (location_x < left)&&((location_x+width) > right))
	{
		return true;
	}
	else return false;
}
bool Shape::insideBounds_y( float top, float bottom)
{
	if((location_y > top)&&((location_y) < bottom)||
	   ((location_y + height)> top)&&((location_y + height) < bottom)||
	   (location_y < top)&&((location_y + height) > bottom)){
		return true;
	}
	else return false;
}

float Shape::getLocation_x()
{
	return location_x;
}

float Shape::getLocation_y()
{
	return location_y;
}

float Shape::getWidth()
{
	return width;
}

float Shape::getHeight()
{
	return height;
}

float Shape::getRed()
{
	return shape_color.red;
}

float Shape::getGreen()
{
	return shape_color.green;
}

float Shape::getBlue()
{
	return shape_color.blue;
}

float Shape::getAlpha()
{
	return shape_color.alpha;
}

void Shape::setVelocity(float vel_x, float vel_y)
{
	velocity_x = vel_x;
	velocity_y = vel_y;
}

void Shape::setColor(float R, float G, float B, float A)
{
	shape_color.red = R;
	shape_color.green = G;
	shape_color.blue = B;
	shape_color.alpha = A;
}
void Shape::setLocation(float x, float y)
{
	location_x = x;
	location_y = y;
}

void Shape::grabShape()
{
	grabbed = true;
}

void Shape::releaseShape()
{
	grabbed = false;
}
bool Shape::isGrabbed()
{
	return grabbed;
}
