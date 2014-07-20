/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ionvif.h
 * Copyright (C) 2014 Watson Xu <xuhuashan@gmail.com>
 *
 * ionvif is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ionvif is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _IONVIF_H_
#define _IONVIF_H_

#include <base_app.h>

G_BEGIN_DECLS

#define IPCAM_TYPE_IONVIF             (ipcam_ionvif_get_type ())
#define IPCAM_IONVIF(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), IPCAM_TYPE_IONVIF, IpcamIOnvif))
#define IPCAM_IONVIF_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), IPCAM_TYPE_IONVIF, IpcamIOnvifClass))
#define IPCAM_IS_IONVIF(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IPCAM_TYPE_IONVIF))
#define IPCAM_IS_IONVIF_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), IPCAM_TYPE_IONVIF))
#define IPCAM_IONVIF_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), IPCAM_TYPE_IONVIF, IpcamIOnvifClass))

typedef struct _IpcamIOnvifClass IpcamIOnvifClass;
typedef struct _IpcamIOnvif IpcamIOnvif;
typedef struct _IpcamIOnvifPrivate IpcamIOnvifPrivate;


struct _IpcamIOnvifClass
{
	IpcamBaseAppClass parent_class;
};

struct _IpcamIOnvif
{
	IpcamBaseApp parent_instance;

	IpcamIOnvifPrivate *priv;
};

GType ipcam_ionvif_get_type (void) G_GNUC_CONST;

gboolean ipcam_ionvif_is_terminating(IpcamIOnvif *ionvif);

gchar *ipcam_ionvif_get_server_addr(IpcamIOnvif *ionvif);

uint ipcam_ionvif_get_server_port(IpcamIOnvif *ionvif);

uint ipcam_ionvif_get_rtsp_port(IpcamIOnvif *ionvif);

G_END_DECLS

#endif /* _IONVIF_H_ */

