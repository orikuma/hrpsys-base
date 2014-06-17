// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
#ifndef __SOFT_BODY_CONTROLLER_SERVICE_H__
#define __SOFT_BODY_CONTROLLER_SERVICE_H__

#include "SoftBodyControllerService.hh"

class SoftBodyController;

class SoftBodyControllerService_impl
	: public virtual POA_OpenHRP::SoftBodyControllerService,
	  public virtual PortableServer::RefCountServantBase
{
public:
	/**
	   \brief constructor
	*/
	SoftBodyControllerService_impl();

	/**
	   \brief destructor
	*/
	virtual ~SoftBodyControllerService_impl();

	void soft_body_controller(TorqueController *i_soft_body_controller);
	
private:
	SoftBodyController *m_soft_body_controller;
};

#endif
