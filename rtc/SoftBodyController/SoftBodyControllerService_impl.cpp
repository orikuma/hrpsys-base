// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
#include <iostream>
#include "SoftBodyControllerService_impl.h"

SoftBodyControllerService_impl::SoftBodyControllerService_impl() : m_soft_body_controller(NULL)
{
}

SoftBodyControllerService_impl::~SoftBodyControllerService_impl()
{
}

void SoftBodyControllerService_impl::soft_body_controller(SoftBodyController *i_soft_body_controller)
{
	m_soft_body_controller = i_soft_body_controller;
}

