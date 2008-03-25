/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc. 
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include "Control.h"

Control::Control()
{
	// Should not be called
	id= -1;
	fd= -1;
}

Control::Control( int id, int fd )
{
	this->fd= fd;
	this->id= id;
}

Control::~Control()
{
}

char* Control::getName()
{
	// Get VideoDevice Current values
	v4l2_queryctrl* tmp= this->queryDevice();
	
	if( tmp != NULL ){
		return (char*)tmp->name;
	}
	
	return NULL;
	
}

int Control::getMin()
{
	// Get VideoDevice Current values
	v4l2_queryctrl* tmp= this->queryDevice();
	
	if( tmp != NULL ){
		return tmp->minimum;
	}
	
	return -1;
}
	
int Control::getMax()
{
	// Get VideoDevice Current values
	v4l2_queryctrl* tmp= this->queryDevice();
	
	if( tmp != NULL ){
		return tmp->maximum;
	}
	
	return -1;
}
	
int Control::getStep()
{
	// Get VideoDevice Current values
	v4l2_queryctrl* tmp= this->queryDevice();
	
	if( tmp != NULL ){
		return tmp->step;
	}
	
	return -1;
}
	
int Control::getValue()
{
	// Get VideoDevice Current values
	v4l2_control *tmpCtrl= (v4l2_control*)calloc(1,sizeof(v4l2_control) );
		
	return tmpCtrl->value;
	
}
	
bool Control::setValue( int value )
{
	
	// Get VideoDevice Current values
	v4l2_queryctrl* tmpQuery= this->queryDevice();
	
	if( tmpQuery != NULL ){
		
		// Validating the new value
		if( !(tmpQuery->maximum >= value && tmpQuery->minimum <= value) ){
			ptracesfl("Value outside range for  ", MT_WARNING, CONTROL_TRACE, false);
			ptracesfl((char*)tmpQuery->name, MT_NONE, CONTROL_TRACE);
			return false;
			free(tmpQuery);
		}
		
		v4l2_control *tmpCtrl= (v4l2_control*)calloc(1,sizeof(v4l2_control) );
		tmpCtrl->id= this->id;
		tmpCtrl->value= value;
	
		// Setting the new value
		if( ioctl(fd, VIDIOC_S_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Set the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		free(tmpCtrl);
		free(tmpQuery);
			
		return true;
			
	}
	
	return false;
	
}

bool Control::increment(){
	
	// Get VideoDevice Current values
	v4l2_queryctrl* tmpQuery= this->queryDevice();
	
	if( tmpQuery != NULL ){
		
		v4l2_control *tmpCtrl= (v4l2_control*)calloc(1,sizeof(v4l2_control) );
		tmpCtrl->id= this->id;
		
		// Getting control id
		if( ioctl(fd, VIDIOC_G_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Get the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		tmpCtrl->value += tmpQuery->step;
		
		// Validating new value
		if( tmpCtrl->value > tmpQuery->maximum ){
			ptracesfl("Already at maximum Value for ", MT_WARNING, CONTROL_TRACE, false);
			ptracesfl((char*)tmpQuery->name, MT_NONE, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		// Setting new value
		if( ioctl(fd, VIDIOC_S_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Set the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
			
		free(tmpCtrl);
		free(tmpQuery);
		
		return true;
		
	}
	
	return false;
	
}
	
bool Control::decrement(){
	
	// Get VideoDevice Current values
	v4l2_queryctrl* tmpQuery= this->queryDevice();
	
	if( tmpQuery != NULL ){
		
		v4l2_control *tmpCtrl= (v4l2_control*)calloc(1,sizeof(v4l2_control) );
		tmpCtrl->id= this->id;
		
		// Getting control id
		if( ioctl(fd, VIDIOC_G_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Get the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		tmpCtrl->value -= tmpQuery->step;
		
		// Validating new value
		if( tmpCtrl->value < tmpQuery->minimum ){
			ptracesfl("Already at minimum Value for ", MT_WARNING, CONTROL_TRACE, false);
			ptracesfl((char*)tmpQuery->name, MT_NONE, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		//Setting new value
		if( ioctl(fd, VIDIOC_S_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Set the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		free(tmpCtrl);
		free(tmpQuery);
			
		return true;
		
	}
	
	return false;
	
}
	
bool Control::reset(){
	
	// Get VideoDevice Current values
	v4l2_queryctrl* tmpQuery= this->queryDevice();
	
	if( tmpQuery != NULL ){
		
		v4l2_control *tmpCtrl= (v4l2_control*)calloc(1,sizeof(v4l2_control) );
		tmpCtrl->id= this->id;
		
		// Getting control id
		if( ioctl(fd, VIDIOC_G_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Get the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
		
		tmpCtrl->value = tmpQuery->default_value;
		
		// Setting new value
		if( ioctl(fd, VIDIOC_S_CTRL, tmpCtrl) == -1 ){
			ptracesfl("Cannot Set the control on the device", MT_ERROR, CONTROL_TRACE);
			free(tmpCtrl);
			free(tmpQuery);
			return false;
		}
			
		free(tmpCtrl);
		free(tmpQuery);
		
		return true;
		
	}
	
	return false;
	
}

v4l2_queryctrl* Control::queryDevice()
{
	
	v4l2_queryctrl* tmpCtrl= (v4l2_queryctrl*)calloc(1,sizeof(v4l2_queryctrl));
	tmpCtrl->id= this->id;
	
	// Get VideoDevice Current values
	if( ioctl (fd, VIDIOC_QUERYCTRL, tmpCtrl) == -1 )
	{
		ptracesfl("Cannot query the control on the device", MT_ERROR, CONTROL_TRACE); 
		return NULL;
	}
	
	return tmpCtrl;
	
}
