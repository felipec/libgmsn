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

#include "sem.h"

GSem*
g_sem_new ()
{
	GSem *sem;
	sem = g_new (GSem, 1);
	sem->mutex = g_mutex_new ();
	sem->cond = g_cond_new ();
	sem->count = 0;

	return sem;
}

void
g_sem_free (GSem *sem)
{
	g_mutex_free (sem->mutex);
	g_cond_free (sem->cond);
}

void
g_sem_up (GSem *sem)
{
	g_mutex_lock (sem->mutex);
	sem->count++;
	g_mutex_unlock (sem->mutex);
	g_cond_signal (sem->cond);
}

void
g_sem_down (GSem* sem)
{
	g_mutex_lock (sem->mutex);
	while (sem->count == 0)
		g_cond_wait (sem->cond, sem->mutex);
	sem->count--;
	g_mutex_unlock (sem->mutex);
}

