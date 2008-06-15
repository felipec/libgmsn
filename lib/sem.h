/*
 * Copyright © 2006 Felipe Contreras.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MSN_SEM_H__
#define __MSN_SEM_H__

typedef struct GSem GSem;

#include <glib.h>

struct GSem
{
	GMutex *mutex;
	GCond *cond;
	gint count;
};

GSem* g_sem_new ();
void g_sem_free (GSem *sem);
void g_sem_up (GSem *sem);
void g_sem_down (GSem* sem);

#endif /* __MSN_SEM_H__ */
