/*
 * onvif-authentication.c
 *
 * Copyright (C) 2014 - Watson Xu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef WITH_OPENSSL
#include "plugin/wsseapi.h"
#endif
#include "soapH.h"
#include "ionvif.h"

int onvif_access_control(struct soap* soap)
{
#ifdef WITH_OPENSSL
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	gchar *username = soap_wsse_get_Username(soap);
	gchar *password = NULL;

	if (!username)
		return soap->error; // no username: return FailedAuthentication

	if (ionvif_get_user_password(ionvif, username, &password)) {
		if (soap_wsse_verify_Password(soap, password))
			return soap->error; // password verification failed: return FailedAuthentic
	}
	else
		return soap_wsse_fault(soap, wsse__FailedAuthentication, NULL);
#endif

	return SOAP_OK;
}
